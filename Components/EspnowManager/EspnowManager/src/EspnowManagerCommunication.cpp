#include "Common.hpp"
#include "IEspnowMessage.hpp"
#include "Payload.hpp"
#include <stdatomic.h>
#include <string.h>

#include "EspnowManager.hpp"
#include "EspnowPeer.hpp"

void EspnowManager::Send(const MacAddress address, const Payload payload)
{
    lowerLayer.Send(address, payload);
}

void EspnowManager::SendBroadcast(const Payload payload)
{
    lowerLayer.SendBroadcast(payload);
}

void EspnowManager::Receive(const MacAddress address, const Payload data)
{
    if(internalState != NOW_RUN)
    {
        return;
    }
    EspnowPeer* sender = NULL;

    Payload message_identifier = Payload(MessageTypeSize);
    Payload message_data = Payload(data);

    message_data >>= message_identifier;

    // Identify the sender
    Enter_Critical_Spinlock(peerListLock);
    for(EspnowPeer* peer : Peers)
    {
        if(peer->IsCorrectAddress(address))
        {
            sender = peer;
            break;
        }
    }

    if(NULL == sender)
    {
        sender = new EspnowPeer(*this, *this, logManager, address);
        Peers.push_back(sender);
    }
    Exit_Critical_Spinlock(peerListLock);

    sender->Receive(&message_identifier, &message_data);
}
