#include "Common.hpp"
#include "IEspnowMessage.hpp"
#include "Payload.hpp"
#include <stdatomic.h>
#include <string.h>

#include "EspnowManager.hpp"

void EspnowManager::Send(const Payload address, const Payload payload)
{
    driver.Send(address, payload);
}

void EspnowManager::Receive(const Payload *src_addr, const Payload* message)
{
    if(internalState != RUN)
    {
        return;
    }

    InterruptReceivedMessageStruct* irms = (InterruptReceivedMessageStruct*)malloc(sizeof(InterruptReceivedMessageStruct));

    irms->src_addr = new Payload(*src_addr);
    irms->message = new Payload(*message);

    Enter_Critical_Spinlock_ISR(InterruptReceivedMessagesSpinlock);
    interruptReceivedMessages.push(irms);
    receivedMessagesCounter++;
    Exit_Critical_Spinlock_ISR(InterruptReceivedMessagesSpinlock);
}

void EspnowManager::HandleReceivedMessages()
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

        delete message_handle->src_addr;
        delete message_handle->message;
        free(message_handle);

        maximumOperationMainFunction--;
    }
}

void EspnowManager::HandleReceivedMessage(const InterruptReceivedMessageStruct* irms)
{
    EspnowPeer* sender = NULL;

    Payload source_address = Payload(*(irms->src_addr));
    Payload message_identifier = Payload(MessageTypeSize);
    Payload message_data = Payload(*(irms->message));

    message_data >>= message_identifier;

    // Identify the sender
    Enter_Critical_Spinlock(peerListLock);
    for(EspnowPeer* peer : Peers)
    {
        if(peer->IsCorrectAddress(source_address.data))
        {
            sender = peer;
            break;
        }
    }

    if(NULL == sender)
    {
        sender = new EspnowPeer(source_address.data, *this, *this, logManager);
        Peers.push_back(sender);
    }
    Exit_Critical_Spinlock(peerListLock);

    sender->Receive(&message_identifier, &message_data);
}
