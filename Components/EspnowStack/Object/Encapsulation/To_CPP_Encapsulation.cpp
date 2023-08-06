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

typedef struct InteruptReceivedMessageStruct 
{
    uint8 src_addr[6];
    Message* message;
    sint16 rx_ctrl_rssi;
}InteruptReceivedMessageStruct;

list<Peer> Peers;

queue<InteruptReceivedMessageStruct*> InteruptReceivedMessages;
Spinlock InteruptReceivedMessagesSpinlock = Spinlock_Init;

void HandleReceivedMessage(InteruptReceivedMessageStruct*);

void To_CPP_Encapsulation_Init(void* pvparrams)
{
    esp_log_level_set("To_CPP_Encapsulation", ESP_LOG_WARN);
 
    Peers = {};
    InteruptReceivedMessages = {};
}

void Send_Cyclic_Msg()
{
    // Send keep alive messages
    RSSI_Message_Keep_Alive::StaticSend();

    // Send needed subsription requests
    for(auto& peer : Peers)
    {
        peer.SendSubsriptionRequest();
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

void MessageReceive(uint8_t *src_addr, Message* message, RSSI_Type rssi)
{
    if(state != RUN)
    {
        return;
    }

    InteruptReceivedMessageStruct* irms = (InteruptReceivedMessageStruct*)malloc(sizeof(InteruptReceivedMessageStruct));

    memcpy(irms->src_addr, src_addr, sizeof(uint8) * 6);
    irms->message = MessageCopy(message);
    irms->rx_ctrl_rssi = rssi;

    Enter_Critical_Spinlock_ISR(InteruptReceivedMessagesSpinlock);
    InteruptReceivedMessages.push(irms);
    Exit_Critical_Spinlock_ISR(InteruptReceivedMessagesSpinlock);
}

void HandleReceivedMessages()
{
    static InteruptReceivedMessageStruct* message_handle;

    while (true) 
    {
        Enter_Critical_Spinlock(InteruptReceivedMessagesSpinlock);
        if(InteruptReceivedMessages.empty())
        {
            Exit_Critical_Spinlock(InteruptReceivedMessagesSpinlock);
            break;
        }
        else
        {
            message_handle = InteruptReceivedMessages.front();
            InteruptReceivedMessages.pop();
            Exit_Critical_Spinlock(InteruptReceivedMessagesSpinlock);
        }
        
        HandleReceivedMessage(message_handle);

        MessageDeinit(message_handle->message);
        free(message_handle);
    }
}

void HandleReceivedMessage(InteruptReceivedMessageStruct* irms)
{
    Peer* sender = NULL;


    Message* message_header = MessageInit(MessageTypeSize);
    Message* message_data = MessageInit(0);
    MessageDecompose(irms->message, message_header, message_data);


    // Indentify the sender

    for(auto& peer : Peers)
    {
        if(peer.IsCorrectAdress(irms->src_addr))
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

    free(message_header);
    free(message_data);
}

size_t GetSeriersRepetitions()
{
    return OpenSeries::numberOfMessagesPerSeries;
}
