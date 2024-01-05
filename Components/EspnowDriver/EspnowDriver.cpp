#include "EspnowDriver.hpp"
#include "WifiDriver.hpp"
#include <string.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>
/* ESPNOW can work in both station and softap mode. */
#if CONFIG_WIFI_AP_ENABLED
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#define GET_MAC_FUNCTION GetMyApMac
#else
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#define GET_MAC_FUNCTION GetMyStaMac
#endif

Spinlock EspnowDriver::sendLock = Spinlock_Init;
std::vector<EspnowDriver*> EspnowDriver::drivers;
MacAddress EspnowDriver::myEspnowMac;
MacAddress EspnowDriver::broadcastEspnowMac;
MacAddress EspnowDriver::communicationChannelEspnowMac;

uint16 EspnowDriver::tuplePoolSize = 10;
std::queue<ReceivedMessageTuple*> EspnowDriver::tuplePool;
std::queue<ReceivedMessageTuple*> EspnowDriver::receivedMessagesQueue;
Spinlock EspnowDriver::receivedMessagesQueueSpinlock = Spinlock_Init;

EspnowDriver::EspnowDriver(LogManager& logManager, TaskManager& taskManager) : logManager(logManager), taskManager(taskManager)
{
    drivers.push_back(this);
    myEspnowMac = WifiDriver::GET_MAC_FUNCTION();
    broadcastEspnowMac = WifiDriver::GetBroadcastMac();
    communicationChannelEspnowMac = WifiDriver::GetBroadcastMac();

    Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
    for(uint16 i = 0; tuplePool.size() + receivedMessagesQueue.size() < tuplePoolSize; i++)
    {
        tuplePool.push(new ReceivedMessageTuple(MacAddress(), RSSI_Type(), int(), NULL));
    }
    Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);
}

EspnowDriver::~EspnowDriver()
{
    drivers.erase(std::remove(drivers.begin(), drivers.end(), this), drivers.end());

    Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
    while (!tuplePool.empty())
    {
        auto tuple = tuplePool.front();
        free(std::get<3>(*tuple));
        delete tuple;
        tuplePool.pop();
    }
    while (!receivedMessagesQueue.empty())
    {
        auto tuple = receivedMessagesQueue.front();
        free(std::get<3>(*tuple));
        delete tuple;
        receivedMessagesQueue.pop();
    }
    Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);
}

void EspnowDriver::Init()
{
    // Set the minimal log level for this component
    logManager.SetMinimalLevel("EspnowDriver", W);

    // Initialize ESPNOW and register receiving callback function.
    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(EspnowDriver::InterruptReceive) );
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_WIFI_CHANNEL;
    peer->ifidx = (wifi_interface_t)ESPNOW_WIFI_IF;
    peer->encrypt = false;
    communicationChannelEspnowMac.CopyTo(peer->peer_addr);

    ESP_ERROR_CHECK( esp_now_add_peer(peer) );

    free(peer);

    // Create tasks for cyclic handling of received messages

    TaskConfig handleReceivedMessages0TaskConfig = TaskConfig(
        "Handle Received Messages on Core 0",
        [this]() { CyclicReceive(); },
        CONFIG_HANDLE_RECEIVED_MESSAGES_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig handleReceivedMessages1TaskConfig = TaskConfig(
        "Handle Received Messages on Core 1",
        [this]() { CyclicReceive(); },
        CONFIG_HANDLE_RECEIVED_MESSAGES_INTERVAL,
        CORE_1,
        8192,
        10
    );

    taskManager.RequestTask(handleReceivedMessages0TaskConfig);
    taskManager.RequestTask(handleReceivedMessages1TaskConfig);
}

void EspnowDriver::Subscribe(IMessageReceiver& messageable)
{
    upperLayer.push_back(&messageable);
}

void EspnowDriver::Send(const MacAddress dst_addr, const std::stack<Payload> payloadStackOriginal)
{
    esp_err_t err;
    // Copy the payload stack to avoid modifying the original
    std::stack<Payload> payloadStack = payloadStackOriginal;

    // Add the destination address to the payload
    payloadStack.push(Payload(dst_addr));


    // Combine the payloads into one data array
    Payload data = Payload::Compose(payloadStack);

    // Send the message
    // Forums said locking the send should help reduce errors
    // https://esp32.com/viewtopic.php?t=17592
    Enter_Critical_Spinlock(sendLock);
    err = esp_now_send(communicationChannelEspnowMac.GetAddress(), data.data, data.GetSize());
    Exit_Critical_Spinlock(sendLock);

    // Check if the message was sent successfully
    if(ESP_OK != err)
    {
        if(ESP_ERR_ESPNOW_NO_MEM == err)
        {
            logManager.Log(E, "EspnowDriver", "esp_now_send: ESP_ERR_ESPNOW_NO_MEM, increase buffer");
        }
        else
        {
            ESP_ERROR_CHECK(err);
        }
    }

}

void EspnowDriver::SendBroadcast(const std::stack<Payload> payloadStack)
{
    Send(broadcastEspnowMac, payloadStack);
}

void EspnowDriver::InterruptReceive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
{
    // Validate the input parameters
    if(ESP_NOW_ETH_ALEN >= len)
    {
        // Received a message with a broken size
        return;
    }
    if(communicationChannelEspnowMac != recv_info->des_addr)
    {
        // Received non broadcast mac
        return;
    }

    // Memory allocation during ISR is not recommended. For that reason
    // we use a pool of tuples to avoid memory allocation during the ISR.
    // Potential issue is that the pool can run out of tuples.
    // In that case, we have to allocate memory during the ISR.
    // The new tuple will be added to the pool after it is handled by the main function.
    // Memory allocation for the data can't be avoided without static memory allocation.

    // If the input parameters are valid, copy the data to in the queue to be read by the main function
    ReceivedMessageTuple* receivedMessageFromInterrupt;

    Enter_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);

    // Check if the tuple pool is empty
    if(!tuplePool.empty())
    {
        // If not, get a tuple from the pool
        receivedMessageFromInterrupt = tuplePool.front();
        tuplePool.pop();
    }
    else
    {
        // Otherwise, we have to allocate a new tuple
        receivedMessageFromInterrupt = new ReceivedMessageTuple(MacAddress(), RSSI_Type(), int(), NULL);
        tuplePoolSize++;
    }

    // Unfortunately, we have to allocate memory for the data here, otherwise we would have to allocate a static buffer

    // Get the data reference from the tuple
    MacAddress& macAddress_ref = std::get<0>(*receivedMessageFromInterrupt);
    RSSI_Type& rssi_ref = std::get<1>(*receivedMessageFromInterrupt);
    int& len_ref = std::get<2>(*receivedMessageFromInterrupt);
    uint8*& data_ref = std::get<3>(*receivedMessageFromInterrupt);

    // Set the data in the tuple
    macAddress_ref.SetAddress(recv_info->src_addr);
    rssi_ref = recv_info->rx_ctrl->rssi;
    len_ref = len;
    data_ref = (uint8*)malloc(len);
    std::memcpy(data_ref, data, len);

    // Add the tuple to the queue
    receivedMessagesQueue.push(receivedMessageFromInterrupt);
    Exit_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
}

void EspnowDriver::CyclicReceive()
{
    uint8 operationsOfCycle = 0xFF;

    while(operationsOfCycle > 0)
    {
        ReceivedMessageTuple* message = NULL;

        MacAddress sourceAddress = MacAddress();
        RSSI_Type rssi = RSSI_Type();
        int len = int();
        uint8* data = NULL;

        // Get a message and data from the queue
        Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
        if(!receivedMessagesQueue.empty())
        {
            // Get the message from the queue
            message = receivedMessagesQueue.front();
            receivedMessagesQueue.pop();

            // Get the data reference from the tuple
            MacAddress& sourceAddress_ref = std::get<0>(*message);
            RSSI_Type& rssi_ref = std::get<1>(*message);
            int& len_ref = std::get<2>(*message);
            uint8*& data_ref = std::get<3>(*message);

            // Copy the data from the tuple
            sourceAddress = sourceAddress_ref;
            rssi = rssi_ref;
            len = len_ref;
            data = (uint8*)malloc(len);
            std::memcpy(data, data_ref, len);

            // Clean the old data
            free(data_ref);

            // Add the tuple to the pool
            tuplePool.push(message);
        }
        Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);

        // If the queue is empty, we need to exit the loop
        if(NULL == message)
        {
            break;
        }

        // Distribute the message to the drivers
        for(auto driver : drivers)
        {
            driver->Receive(sourceAddress, rssi, data, len);
        }

        free(data);

        operationsOfCycle--;
    }
}

void EspnowDriver::Receive(const MacAddress sourceAddress, const RSSI_Type rssi, const uint8* data, int len)
{
    // Extract payloads from the data
    std::queue<Payload> payloadQueue = Payload::Decompose(Payload(data, len));

    // Check if the queue is empty
    if(payloadQueue.empty())
    {
        return;
    }

    // Get the destination address
    MacAddress destinationAddress = payloadQueue.front();
    payloadQueue.pop();

    // Check if the destination address is our address or the broadcast address
    if(destinationAddress == broadcastEspnowMac
    || destinationAddress == myEspnowMac)
    {
        // Extract the RSSI
        Payload rssiPayload = Payload((uint8*)(&rssi), sizeof(rssi));

        // Add the RSSI to the payload queue
        payloadQueue.push(rssiPayload);

        // Send the message to the upper layer
        for(auto messageable : upperLayer)
        {
            messageable->Receive(sourceAddress, payloadQueue);
        }
    }
}
