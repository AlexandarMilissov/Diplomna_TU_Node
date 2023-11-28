#include "Common.h"
#include "EspnowDriver.h"
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

std::vector<Peer*> Peers;
Spinlock peerListProtection = Spinlock_Init;

void SendMessage(const uint8* dst_addr, MessageType messageType, const Message* data)
{
    Message* header = MessageInit(MessageTypeSize);
    *(header->data) = (uint8)messageType;

    Message* message = MessageCompose(header, data);

    DataSend(dst_addr, message);

    MessageDeinit(message);
    MessageDeinit(header);
}

void ReceiveMessage(const uint8_t *src_addr, const Message* message, const RSSI_Type rssi)
{
    if(espnowManagerInternalState != RUN)
    {
        return;
    }

    InterruptReceivedMessageStruct* irms = (InterruptReceivedMessageStruct*)malloc(sizeof(InterruptReceivedMessageStruct));

    memcpy(irms->src_addr, src_addr, sizeof(uint8) * 6);
    irms->message = MessageCopy(message);
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
    for(Peer* peer : Peers)
    {
        if(peer->IsCorrectAddress(irms->src_addr))
        {
            sender = peer;
            break;
        }
    }

    if(NULL == sender)
    {
        sender = new Peer(irms->src_addr);
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
    catch(const std::invalid_argument& e)
    {
        ESP_LOGE("EspnowManager", "ESP_ERR_INVALID_ARG: %s\n", e.what());
    }

    MessageDeinit(message_header);
    MessageDeinit(message_data);
}