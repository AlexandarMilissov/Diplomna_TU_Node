#include "To_CPP_Encapsulation.hpp"
#include "EspnowManager.h"
#include "Espnow_Message_General.h"

#include "Peer.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"

#include <list>
#include <algorithm>
#include <queue>
#include <cstring>
#include <stdexcept>

using namespace std;

typedef struct InterruptReceivedMessageStruct 
{
    uint8 src_addr[6];
    Message* message;
    sint16 rx_ctrl_rssi;
}InterruptReceivedMessageStruct;

list<Peer> Peers;
Spinlock peerListProtection = Spinlock_Init;

queue<InterruptReceivedMessageStruct*> InterruptReceivedMessages;
Spinlock InterruptReceivedMessagesSpinlock = Spinlock_Init;
uint64 handledMessagesCounter = 0;
uint64 receivedMessagesCounter = 0;

void HandleReceivedMessage(const InterruptReceivedMessageStruct*);

void To_CPP_Encapsulation_Init(void* pvparrams)
{
    esp_log_level_set("To_CPP_Encapsulation", ESP_LOG_WARN);
 
    Peers = {};
    InterruptReceivedMessages = {};
}

void Send_Cyclic_Msg()
{
    // Send keep alive messages
    RSSI_Message_Keep_Alive::StaticSend();

    // Send needed subscription requests
    for(auto& peer : Peers)
    {
        peer.SendSubscriptionRequest();
    }
}

void SeriesSend()
{
    RSSI_Message_Calculation::StaticSend();
}

void UpdateSeries()
{
    for(auto& peer : Peers)
    {
        peer.UpdateSeries();
    }
}

void EncapsulationMonitor()
{
    size_t operations;
    uint64 localHandledMessagesCounter;
    uint64 localReceivedMessagesCounter;

    Enter_Critical_Spinlock(InterruptReceivedMessagesSpinlock);
    operations = InterruptReceivedMessages.size();
    localHandledMessagesCounter = handledMessagesCounter;
    localReceivedMessagesCounter = receivedMessagesCounter;
    Exit_Critical_Spinlock(InterruptReceivedMessagesSpinlock);

    ESP_LOGI("Monitor", "[%d] queued messages", operations);
    ESP_LOGI("Monitor", "[%lld] handled messages", localHandledMessagesCounter);
    ESP_LOGI("Monitor", "[%lld] received messages", localReceivedMessagesCounter);

    for(auto& peer : Peers)
    {
        ESP_LOGI("Monitor", "----->");
        peer.Monitor();
    }
}

void MessageReceive(const uint8_t *src_addr, const Message* message, const RSSI_Type rssi)
{
    if(state != RUN)
    {
        return;
    }

    InterruptReceivedMessageStruct* irms = (InterruptReceivedMessageStruct*)malloc(sizeof(InterruptReceivedMessageStruct));

    memcpy(irms->src_addr, src_addr, sizeof(uint8) * 6);
    irms->message = MessageCopy(message);
    irms->rx_ctrl_rssi = rssi;

    Enter_Critical_Spinlock_ISR(InterruptReceivedMessagesSpinlock);
    InterruptReceivedMessages.push(irms);
    receivedMessagesCounter++;
    Exit_Critical_Spinlock_ISR(InterruptReceivedMessagesSpinlock);
}

void HandleReceivedMessages()
{
    InterruptReceivedMessageStruct* message_handle;
    uint8 maximumOperationMainFunction = 0xFF;

    while (maximumOperationMainFunction > 0) 
    {
        message_handle = NULL;

        Enter_Critical_Spinlock(InterruptReceivedMessagesSpinlock);
        if(!InterruptReceivedMessages.empty())
        {
            handledMessagesCounter++;
            message_handle = InterruptReceivedMessages.front();
            InterruptReceivedMessages.pop();
        }
        Exit_Critical_Spinlock(InterruptReceivedMessagesSpinlock);

        if(NULL == message_handle)
        {
            break;
        }
        
        HandleReceivedMessage(message_handle);

        MessageDeinit(message_handle->message);
        free(message_handle);

        maximumOperationMainFunction--;
    }
}

void HandleReceivedMessage(const InterruptReceivedMessageStruct* irms)
{
    Peer* sender = NULL;


    Message* message_header = MessageInit(MessageTypeSize);
    Message* message_data = MessageInit(0);
    MessageDecompose(irms->message, message_header, message_data);


    // Identify the sender
    Enter_Critical_Spinlock(peerListProtection);
    for(auto& peer : Peers)
    {
        if(peer.IsCorrectAddress(irms->src_addr))
        {
            sender = &peer;
            break;
        }
    }
    
    if(NULL == sender)
    {
        sender = new Peer(irms->src_addr);
        Peers.push_back(*sender);
    }
    Exit_Critical_Spinlock(peerListProtection);  
    
    // Proccess the message
    try
    {
        switch (*(message_header->data))
        {
        case RSSI_REQUEST:
        {
            RSSI_Message_Request RSSI_Message = RSSI_Message_Request(*message_data);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_CALCULATION:
        {
            RSSI_Message_Calculation RSSI_Message = RSSI_Message_Calculation(irms->rx_ctrl_rssi, *message_data);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_KEEP_ALIVE:
        {
            RSSI_Message_Keep_Alive RSSI_Message = RSSI_Message_Keep_Alive(irms->rx_ctrl_rssi);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_ACKNOWLEDGE:
        {
            RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(message_data);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        default:
            break;
        }
    }
    catch(const invalid_argument& e)
    {
        ESP_LOGE("To_CPP_Encapsulation", "ESP_ERR_INVALID_ARG: %s\n", e.what());
    }

    MessageDeinit(message_header);
    MessageDeinit(message_data);
}

size_t GetSeriesRepetitions()
{
    return OpenSeries::numberOfMessagesPerSeries;
}
