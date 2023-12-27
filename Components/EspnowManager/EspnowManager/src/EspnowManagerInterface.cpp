#include "Common.hpp"
#include <stdatomic.h>

#include "EspnowManager.hpp"

EspnowManager::EspnowManager(
    IDriver& driver,
    LogManager& logManager,
    IScheduler& taskManager
) :
    driver(driver),
    logManager(logManager),
    taskManager(taskManager)
{
    for(uint16 i = 0; i < tuplePoolSize; i++)
    {
        tuplePool.push(new std::tuple<Payload*, Payload*>(NULL, NULL));
    }
}

EspnowManager::~EspnowManager()
{
    Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
    for(auto& peer : Peers)
    {
        delete peer;
    }
    while (!tuplePool.empty())
    {
        auto tuple = tuplePool.front();
        delete std::get<0>(*tuple);
        delete std::get<1>(*tuple);
        delete tuple;
        tuplePool.pop();
    }
    while (!receivedMessagesQueue.empty())
    {
        auto tuple = receivedMessagesQueue.front();
        delete std::get<0>(*tuple);
        delete std::get<1>(*tuple);
        delete tuple;
        receivedMessagesQueue.pop();
    }
    Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);
}

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