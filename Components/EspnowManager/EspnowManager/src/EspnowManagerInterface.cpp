#include "Common.hpp"
#include <stdatomic.h>

#include "EspnowManager.hpp"

void EspnowManager::ActivateNetwork()
{
    internalState = RUN;
}

void EspnowManager::DeactivateNetwork()
{
    internalState = INIT;
}

void EspnowManager::Subscribe()
{
    calculationSubscribers++;
}

void EspnowManager::Unsubscribe()
{
    calculationSubscribers--;
}