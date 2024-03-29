#include "Common.hpp"
#include <stdatomic.h>

#include "EspnowManager.hpp"
#include "EspnowPeer.hpp"

EspnowManager::EspnowManager(
    LogManager& logManager,
    IScheduler& taskManager,
    IMessageSender& lowerLayer
) :
    logManager(logManager),
    taskManager(taskManager),
    lowerLayer(lowerLayer)
{}

EspnowManager::~EspnowManager()
{
    Enter_Critical_Spinlock(peerListLock);
    for(auto& peer : espnowPeers)
    {
        delete peer;
    }
    espnowPeers.clear();
    Exit_Critical_Spinlock(peerListLock);
}

void EspnowManager::ActivateNetwork()
{
    internalState = NOW_RUN;
}

void EspnowManager::DeactivateNetwork()
{
    internalState = NOW_INIT;
}

void EspnowManager::Subscribe()
{
    calculationSubscribers++;
}

void EspnowManager::Unsubscribe()
{
    calculationSubscribers--;
}