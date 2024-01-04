#include "EspnowDriver.hpp"
#include "WifiDriver.hpp"
#include <string.h>
#include <cstring>
#include <algorithm>
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
        esp_now_recv_info_t* recv_info = (esp_now_recv_info_t*)malloc(sizeof(esp_now_recv_info_t));
        uint8* data = NULL;
        tuplePool.push(new ReceivedMessageTuple(recv_info, data, int()));
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
        free(std::get<0>(*tuple));
        free(std::get<1>(*tuple));
        delete tuple;
        tuplePool.pop();
    }
    while (!receivedMessagesQueue.empty())
    {
        auto tuple = receivedMessagesQueue.front();
        free(std::get<0>(*tuple));
        free(std::get<1>(*tuple));
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
        []() { CyclicReceive(); },
        CONFIG_HANDLE_RECEIVED_MESSAGES_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig handleReceivedMessages1TaskConfig = TaskConfig(
        "Handle Received Messages on Core 1",
        []() { CyclicReceive(); },
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
    upperLayerMessageables.push_back(&messageable);
}

void EspnowDriver::Send(const MacAddress dst_addr, const Payload message)
{
    esp_err_t err;

    Payload package = Payload(dst_addr);
    package += message;

    // Forums said locking the send should help reduce errors
    // https://esp32.com/viewtopic.php?t=17592

    uint8 communication_channel_mac[6];
    communicationChannelEspnowMac.CopyTo(communication_channel_mac);

    Enter_Critical_Spinlock(sendLock);

    err = esp_now_send(communication_channel_mac, package.data, package.GetSize());

    Exit_Critical_Spinlock(sendLock);

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

void EspnowDriver::SendBroadcast(const Payload message)
{
    Send(broadcastEspnowMac, message);
}

void EspnowDriver::InterruptReceive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
{
    // Validate the input parameters
    if(ESP_NOW_ETH_ALEN >= len)
    {
        // Received a message with a broken size
        return;
    }
    MacAddress received_address(recv_info->src_addr);
    if(received_address != communicationChannelEspnowMac)
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
        esp_now_recv_info_t* recv_info = (esp_now_recv_info_t*)malloc(sizeof(esp_now_recv_info_t));
        uint8* data = NULL;
        receivedMessageFromInterrupt = new ReceivedMessageTuple(recv_info, data, int());
        tuplePoolSize++;
    }

    Exit_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);

    // Copy the data to the tuple
    std::memcpy(std::get<0>(*receivedMessageFromInterrupt), recv_info, sizeof(esp_now_recv_info_t));
    // Unfortunately, we have to allocate memory for the data here, otherwise we would have to allocate a static buffer
    std::get<1>(*receivedMessageFromInterrupt) = (uint8*)malloc(len);
    std::memcpy(std::get<1>(*receivedMessageFromInterrupt), data, len);
    std::get<2>(*receivedMessageFromInterrupt) = len;

    // Add the tuple to the queue
    Enter_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
    receivedMessagesQueue.push(receivedMessageFromInterrupt);
    Exit_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
}

void EspnowDriver::CyclicReceive()
{
    ReceivedMessageTuple* message = NULL;
    uint8 operationsOfCycle = 0xFF;

    esp_now_recv_info_t* recv_info = (esp_now_recv_info_t*)malloc(sizeof(esp_now_recv_info_t));
    uint8* data = NULL;
    int len = int();

    while(operationsOfCycle > 0)
    {
        message = NULL;

        // Get a message and data from the queue
        Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
        if(!receivedMessagesQueue.empty())
        {
            // Get the message from the queue
            message = receivedMessagesQueue.front();
            receivedMessagesQueue.pop();

            // Get the data from the tuple
            len = std::get<2>(*message);
            data = (uint8*)malloc(len);
            std::memcpy(data, std::get<1>(*message), len);
            std::memcpy(recv_info, std::get<0>(*message), sizeof(esp_now_recv_info_t));

            // Clean the old data
            free(std::get<1>(*message));

            // Add the tuple to the pool
            tuplePool.push(message);
        }
        Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);

        // If the queue is empty, we need to exit the loop
        if(NULL == message)
        {
            break;
        }

        // Handle the data
        // TODO: FIX THIS

        Payload received_message_data = Payload(data, len);

        Payload destination_address = Payload(ESP_NOW_ETH_ALEN);
        received_message_data >>= destination_address;

        MacAddress destination_mac_address(destination_address);

        if(destination_mac_address == communicationChannelEspnowMac
        || destination_mac_address == myEspnowMac)
        {
            MacAddress source_address = MacAddress(recv_info->src_addr);
            RSSI_Type rssi = recv_info->rx_ctrl->rssi;
            Payload rssi_payload = Payload((uint8*)(&rssi), sizeof(RSSI_Type));
            received_message_data += rssi_payload;

            for (auto driver : drivers)
            {
                driver->Receive(source_address, received_message_data);
            }
        }

        free(data);
    }
    free(recv_info);
}

void EspnowDriver::Receive(const MacAddress address, const Payload message)
{
    for(auto messageable : upperLayerMessageables)
    {
        messageable->Receive(address, message);
    }
}
