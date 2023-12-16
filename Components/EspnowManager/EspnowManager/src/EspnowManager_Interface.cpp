#include "Common.hpp"
#include "EspnowManager_Interface.hpp"
#include "EspnowManager_Internal.hpp"
#include "EspnowDriver.hpp"
#include <stdatomic.h>

std::atomic<EspnowManagerInternalState> espnowManagerInternalState = NO_INIT;
std::atomic<uint16> calculationSubscribers = 0;

void EspnowManager_ActivateNetwork()
{
    espnowManagerInternalState = RUN;
}

void EspnowManager_DeactivateNetwork()
{
    espnowManagerInternalState = INIT;
}

void EspnowManager_Subscribe()
{
    calculationSubscribers++;
}

void EspnowManager_Unsubscribe()
{
    calculationSubscribers--;
}