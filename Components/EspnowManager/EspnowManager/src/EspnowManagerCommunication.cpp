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
    std::tuple<Payload*, Payload*>* irms = new std::tuple<Payload*, Payload*>(new Payload(*src_addr), new Payload(*message));

    Enter_Critical_Spinlock_ISR(receivedMessagesQueueSpinlock);
    receivedMessagesQueue.push(irms);
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
        free(message_handle);

        maximumOperationMainFunction--;
    }
}

void EspnowManager::HandleReceivedMessage(Payload* src_address, Payload* msg_data)
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
