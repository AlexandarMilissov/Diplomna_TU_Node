#include "Common.hpp"
#include "TaskManager.hpp"
#include "Payload.hpp"
#include "OpenSeries.hpp"
#include <string>

#include "EspnowManager.hpp"
#include "WifiDriver.hpp"

#if (CONFIG_NUMBER_OF_MESSAGES_PER_SERIES * CONFIG_SERIES_CYCLIC_SEND_INTERVAL) >= CONFIG_SERIES_INITIATION_INTERVAL
    #error "Invalid send configuration. Overlapping send times."
#endif

#if (CONFIG_UPDATE_PEERS_INTERVAL * CONFIG_LIFE_OF_SERIES) <= (CONFIG_NUMBER_OF_MESSAGES_PER_SERIES * CONFIG_SERIES_CYCLIC_SEND_INTERVAL)
    #error "The series will never complete successfully."
#endif

#if (CONFIG_UPDATE_PEERS_INTERVAL * CONFIG_LIFE_OF_PEERS) <= CONFIG_SEND_KEEP_ALIVE_INTERVAL
    #error "The peers are killed before they can send a new message."
#endif

void EspnowManager::Init()
{
    Peers = {};
    receivedMessagesQueue = {};

    logManager.SetMinimalLevel("EspnowManager", I);

    driver.Subscribe(*this);

    TaskConfig updatePeersTaskConfig = TaskConfig(
        "Update Series",
        [this]() { MainFunctionUpdatePeers(); },
        CONFIG_UPDATE_PEERS_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig sendKeepAliveTaskConfig = TaskConfig(
        "Send Keep Alive",
        [this]() { MainFunctionSendCyclicKeepAlive(); },
        CONFIG_SEND_KEEP_ALIVE_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig sendCalculationTaskConfig = TaskConfig(
        "Series Begin",
        [this]() { MainFunctionSeriesBegin(); },
        CONFIG_SERIES_INITIATION_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig HandleReceivedMessages0TaskConfig = TaskConfig(
        "Handle Received Messages on Core 0",
        [this]() { MainFunctionHandleReceivedMessages(); },
        CONFIG_HANDLE_RECEIVED_MESSAGES_INTERVAL,
        CORE_0,
        8192,
        10
    );
    TaskConfig HandleReceivedMessages1TaskConfig = TaskConfig(
        "Handle Received Messages on Core 1",
        [this]() { MainFunctionHandleReceivedMessages(); },
        CONFIG_HANDLE_RECEIVED_MESSAGES_INTERVAL,
        CORE_1,
        8192,
        10
    );
    taskManager.RequestTask(updatePeersTaskConfig);
    taskManager.RequestTask(sendKeepAliveTaskConfig);
    taskManager.RequestTask(sendCalculationTaskConfig);
    taskManager.RequestTask(HandleReceivedMessages0TaskConfig);
    taskManager.RequestTask(HandleReceivedMessages1TaskConfig);

    internalState = NOW_INIT;
}

void EspnowManager::MainFunctionUpdatePeers()
{
    std::vector<EspnowPeer*> deadPeers;

    Enter_Critical_Spinlock(peerListLock);
    for(auto peer : Peers)
    {
        if(!peer->IsAlive())
        {
            deadPeers.push_back(peer);
        }
    }
    for(auto peer : deadPeers)
    {
        delete peer;
        Peers.erase(std::remove(Peers.begin(), Peers.end(), peer), Peers.end());
    }
    Exit_Critical_Spinlock(peerListLock);

    for (auto peer : Peers)
    {
        peer->Refresh();
    }

    if (deadPeers.size() > 0)
    {
        logManager.Log(W, "EspnowManager", "[%d] EspnowPeers have disconnected.", deadPeers.size());
    }
}

void EspnowManager::MainFunctionSendCyclicKeepAlive()
{
    if(NOW_RUN != internalState)
    {
        return;
    }

    // Send keep alive messages
    EspnowMessageKeepAlive keepAlive;
    SendBroadcast(keepAlive.GetPayload());

    // Send needed subscription requests
    for(auto& peer : Peers)
    {
        peer->SendSubscriptionRequest();
    }
}

void EspnowManager::MainFunctionSeriesBegin()
{
    if(NOW_RUN != internalState)
    {
        return;
    }
    if(0 < calculationSubscribers)
    {
        TaskConfig taskConfig = TaskConfig
        (
            "Send Calculation",
            [this]() { SendCalculationSeries(); },
            CONFIG_SERIES_CYCLIC_SEND_INTERVAL,
            CORE_1,
            8192,
            20,
            true,
            CONFIG_NUMBER_OF_MESSAGES_PER_SERIES
        );
        taskManager.RequestTask(taskConfig);
    }
}

void EspnowManager::MainFunctionHandleReceivedMessages()
{
    if(NOW_RUN != internalState)
    {
        return;
    }

    HandleReceivedMessages();
}

void EspnowManager::SendCalculationSeries()
{
    if(NOW_RUN != internalState)
    {
        return;
    }

    EspnowMessageCalculation calculation;

    SendBroadcast(calculation.GetPayload());
}

std::string EspnowManager::GetMonitorData()
{
    static std::string messagesLog;
    size_t operations;
    uint64 localHandledMessagesCounter;
    uint64 localReceivedMessagesCounter;
    uint16 localTuplePoolSize;

    Enter_Critical_Spinlock(receivedMessagesQueueSpinlock);
    operations = receivedMessagesQueue.size();
    localHandledMessagesCounter = handledMessagesCounter;
    localReceivedMessagesCounter = receivedMessagesCounter;
    localTuplePoolSize = tuplePoolSize;
    Exit_Critical_Spinlock(receivedMessagesQueueSpinlock);

    messagesLog = "\n";
    messagesLog += "[" + std::to_string(localTuplePoolSize          ) + "] tuple queue size\n"  ;
    messagesLog += "[" + std::to_string(operations                  ) + "] queued messages\n"   ;
    messagesLog += "[" + std::to_string(localHandledMessagesCounter ) + "] handled messages\n"  ;
    messagesLog += "[" + std::to_string(localReceivedMessagesCounter) + "] received messages\n" ;

    for(auto& peer : Peers)
    {
        messagesLog += peer->GetMonitorData();
    }

    return messagesLog;
}
