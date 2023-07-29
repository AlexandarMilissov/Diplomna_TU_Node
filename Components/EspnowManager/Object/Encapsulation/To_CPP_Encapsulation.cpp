#include "To_CPP_Encapsulation.hpp"

#include "Peer.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Interface.hpp"

#include <list>
#include <algorithm>
#include <queue>

using namespace std;

typedef struct InteruptReceivedMessageStruct 
{
    uint8 src_addr[6];
    MessageStruct* messageStruct;
    size_t size;
    sint16 rx_ctrl_rssi;
}InteruptReceivedMessageStruct;

list<Peer> Peers;

queue<InteruptReceivedMessageStruct*> InteruptReceivedMessages;
Spinlock InteruptReceivedMessagesSpinlock = Spinlock_Init;

void RSSI_MessageHandle(InteruptReceivedMessageStruct);

void To_CPP_Encapsulation_Init(void* pvparrams)
{
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
    RSSI_Message_Calculation::StaticSend(NULL);
    TaskSleepMiliSeconds(1);
}

void UpdateSeries()
{
    for(auto& peer : Peers)
    {
        peer.UpdateSeries();
    }
}

esp_err_t RSSI_OnMessageReceive(uint8_t *src_addr, MessageStruct* messageStruct, size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl)
{
    InteruptReceivedMessageStruct* irms = (InteruptReceivedMessageStruct*)malloc(sizeof(InteruptReceivedMessageStruct));

    memcpy(irms->src_addr, src_addr, sizeof(uint8) * 6);
    irms->messageStruct = messageStruct;
    irms->size = size;
    irms->rx_ctrl_rssi = rx_ctrl->rssi;

    Enter_Critical_Spinlock_ISR(InteruptReceivedMessagesSpinlock);
    InteruptReceivedMessages.push(irms);
    Exit_Critical_Spinlock_ISR(InteruptReceivedMessagesSpinlock);

    return ESP_OK;
}

void HandleReceivedMessages()
{
    static InteruptReceivedMessageStruct copy;
    static InteruptReceivedMessageStruct* original;

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
            original = InteruptReceivedMessages.front();
            InteruptReceivedMessages.pop();
            Exit_Critical_Spinlock(InteruptReceivedMessagesSpinlock);
        }


        copy = *original;
        
        RSSI_MessageHandle(copy);

        free(original->messageStruct->message);
        free(original->messageStruct);
        free(original);
    }
}

void RSSI_MessageHandle(InteruptReceivedMessageStruct irms)
{
    Peer* sender = NULL;

    // Indentify the sender

    for(auto& peer : Peers)
    {
        if(peer.IsCorrectAdress(irms.src_addr))
        {
            sender = &peer;
            break;
        }
    }
    
    if(NULL == sender)
    {
        sender = new Peer(irms.src_addr);
        Peers.push_back(*sender);
    }
    
    // Proccess the message
    try
    {
        switch (irms.messageStruct->messageType)
        {
        case RSSI_REQUEST:
        {
            RSSI_Message_Request RSSI_Message = RSSI_Message_Request(irms.messageStruct);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_CALCULATION:
        {
            RSSI_Message_Calculation RSSI_Message = RSSI_Message_Calculation(irms.rx_ctrl_rssi, *(irms.messageStruct));
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_KEEP_ALIVE:
        {
            RSSI_Message_Keep_Alive RSSI_Message = RSSI_Message_Keep_Alive(irms.rx_ctrl_rssi);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_ACKNOWLEDGE:
        {
            RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(*(irms.messageStruct));
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        default:
            break;
        }
    }
    catch(const ValidationFailedException& e)
    {
        //printf("ESP_ERR_INVALID_ARG\n");
    }
}
