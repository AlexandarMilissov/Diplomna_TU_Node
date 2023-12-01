#include "Common.h"
#include "EspnowDriver.hpp"
#include "EspnowManager_Interface.hpp"
#include "EspnowManager_Internal.hpp"
#include "EspnowManager_Communication.hpp"
#include <stdatomic.h>
#include <string.h>
#include <stdexcept>

void HandleReceivedMessage(const InterruptReceivedMessageStruct*);

uint64 handledMessagesCounter = 0;
uint64 receivedMessagesCounter = 0;
std::queue<InterruptReceivedMessageStruct*> interruptReceivedMessages;
Spinlock InterruptReceivedMessagesSpinlock = Spinlock_Init;

std::vector<EspnowPeer*> Peers;
Spinlock peerListProtection = Spinlock_Init;

void SendMessage(const uint8* dst_addr, MessageType messageType, const Payload* data)
{
    Payload* message = new Payload(MessageTypeSize);
    *(message->data) = messageType;

    if(NULL != data)
    {
        *message += *data;
    }

    DataSend(dst_addr, message);

    delete message;
}

void ReceiveMessage(const uint8_t *src_addr, const Payload* message, const RSSI_Type rssi)
{
    if(espnowManagerInternalState != RUN)
    {
        return;
    }

    InterruptReceivedMessageStruct* irms = (InterruptReceivedMessageStruct*)malloc(sizeof(InterruptReceivedMessageStruct));

    memcpy(irms->src_addr, src_addr, sizeof(uint8) * 6);
    irms->message = new Payload(*message);
    irms->rx_ctrl_rssi = rssi;

    Enter_Critical_Spinlock_ISR(InterruptReceivedMessagesSpinlock);
    interruptReceivedMessages.push(irms);
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
        if(!interruptReceivedMessages.empty())
        {
            handledMessagesCounter++;
            message_handle = interruptReceivedMessages.front();
            interruptReceivedMessages.pop();
        }
        Exit_Critical_Spinlock(InterruptReceivedMessagesSpinlock);

        if(NULL == message_handle)
        {
            break;
        }

        HandleReceivedMessage(message_handle);

        delete message_handle->message;
        free(message_handle);

        maximumOperationMainFunction--;
    }
}

void HandleReceivedMessage(const InterruptReceivedMessageStruct* irms)
{
    EspnowPeer* sender = NULL;

    Payload* message_header = new Payload(MessageTypeSize);
    Payload* message_data = new Payload(*(irms->message));

    *message_data >>= *message_header;

    // Identify the sender
    Enter_Critical_Spinlock(peerListProtection);
    for(EspnowPeer* peer : Peers)
    {
        if(peer->IsCorrectAddress(irms->src_addr))
        {
            sender = peer;
            break;
        }
    }

    if(NULL == sender)
    {
        sender = new EspnowPeer(irms->src_addr);
        Peers.push_back(sender);
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
            sender->ReceiveMessage(RSSI_Message);
        }
        break;
        case RSSI_CALCULATION:
        {
            RSSI_Message_Calculation RSSI_Message = RSSI_Message_Calculation(irms->rx_ctrl_rssi, *message_data);
            sender->ReceiveMessage(RSSI_Message);
        }
        break;
        case RSSI_KEEP_ALIVE:
        {
            RSSI_Message_Keep_Alive RSSI_Message = RSSI_Message_Keep_Alive(irms->rx_ctrl_rssi);
            sender->ReceiveMessage(RSSI_Message);
        }
        break;
        case RSSI_ACKNOWLEDGE:
        {
            RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(message_data);
            sender->ReceiveMessage(RSSI_Message);
        }
        break;
        default:
            break;
        }
    }
    catch(const std::invalid_argument& e)
    {
        LogManager_Log(E, "EspnowManager", "ESP_ERR_INVALID_ARG: %s\n", e.what());
    }

    delete message_header;
    delete message_data;
}