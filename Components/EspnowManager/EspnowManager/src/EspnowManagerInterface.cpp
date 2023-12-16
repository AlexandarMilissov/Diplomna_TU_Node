#include "Common.hpp"
#include "EspnowDriver.hpp"
#include <stdatomic.h>

#include "EspnowManager.hpp"

std::atomic<EspnowManagerInternalState> EspnowManager::espnowManagerInternalState = NO_INIT;
std::atomic<uint16> EspnowManager::calculationSubscribers = 0;

void EspnowManager::ActivateNetwork()
{
    espnowManagerInternalState = RUN;
}

void EspnowManager::DeactivateNetwork()
{
    espnowManagerInternalState = INIT;
}

void EspnowManager::Subscribe()
{
    calculationSubscribers++;
}

void EspnowManager::Unsubscribe()
{
    calculationSubscribers--;
}