#include "Common.hpp"
#include "IEspnowMessage.hpp"
#include "Payload.hpp"
#include <stdatomic.h>
#include <string.h>

#include "EspnowManager.hpp"
#include "EspnowPeer.hpp"

void EspnowManager::Receive(const MacAddress address, const std::queue<Payload> payloadQueue)
{
    if(internalState != NOW_RUN)
    {
        return;
    }
    EspnowPeer* sender = NULL;

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
        sender = new EspnowPeer(lowerLayer, *this, logManager, address);
        Peers.push_back(sender);
    }
    Exit_Critical_Spinlock(peerListLock);

    sender->Receive(payloadQueue);
}
