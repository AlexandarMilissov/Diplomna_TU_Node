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
    if(internalState != NOW_RUN)
    {
        return;
    }

    std::tuple<Payload*, Payload*>* receivedMessageFromInterrupt;
    // Memory allocation during ISR is not recommended.
    // We use a pool of tuples to avoid memory allocation during ISR.
    // But if the pool is empty, we have to allocate memory.
    // Memory allocation for the payloads can't be avoided.

    Enter_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
    if(!tuplePool.empty())
    {
        receivedMessageFromInterrupt = tuplePool.front();
        tuplePool.pop();
    }
    else
    {
        receivedMessageFromInterrupt = new std::tuple<Payload*, Payload*>(NULL, NULL);
        tuplePoolSize++;
    }
    Exit_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);

    std::get<0>(*receivedMessageFromInterrupt) = new Payload(*src_addr);
    std::get<1>(*receivedMessageFromInterrupt) = new Payload(*message);

    Enter_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
    receivedMessagesQueue.push(receivedMessageFromInterrupt);
    receivedMessagesCounter++;
    Exit_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
}

void EspnowManager::HandleReceivedMessages()
{
    std::tuple<Payload*, Payload*>* message_handle = NULL;
    uint8 maximumOperationMainFunction = 0xFF;

    while (maximumOperationMainFunction > 0)
    {
        message_handle = NULL;

        Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
        if(!receivedMessagesQueue.empty())
        {
            handledMessagesCounter++;
            message_handle = receivedMessagesQueue.front();
            receivedMessagesQueue.pop();
        }
        Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);

        if(NULL == message_handle)
        {
            break;
        }

        Payload* src_addr = std::get<0>(*message_handle);
        Payload* message = std::get<1>(*message_handle);

        HandleReceivedMessage(src_addr, message);

        delete src_addr;
        delete message;

        Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
        tuplePool.push(message_handle);
        Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);

        maximumOperationMainFunction--;
    }
}

void EspnowManager::HandleReceivedMessage(const Payload* src_address, const Payload* msg_data)
{
    EspnowPeer* sender = NULL;

    Payload source_address = Payload(*(src_address));
    Payload message_identifier = Payload(MessageTypeSize);
    Payload message_data = Payload(*(msg_data));

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
