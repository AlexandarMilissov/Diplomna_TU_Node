#include "Common.hpp"
#include "TaskManager.hpp"
#include "Payload.hpp"
#include "OpenSeries.hpp"
#include <string>
#include "EspnowPeer.hpp"

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

    logManager.SetMinimalLevel("EspnowManager", I);

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

    taskManager.RequestTask(updatePeersTaskConfig);
    taskManager.RequestTask(sendKeepAliveTaskConfig);
    taskManager.RequestTask(sendCalculationTaskConfig);

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
    std::stack<Payload> payloadStack = keepAlive.GetPayload();
    SendBroadcast(payloadStack);

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

void EspnowManager::SendCalculationSeries()
{
    if(NOW_RUN != internalState)
    {
        return;
    }

    EspnowMessageCalculation calculation;

    std::stack<Payload> payloadStack = calculation.GetPayload();

    SendBroadcast(payloadStack);
}

std::string EspnowManager::GetMonitorData()
{
    std::string messagesLog = "";

    for(auto& peer : Peers)
    {
        messagesLog += peer->GetMonitorData();
    }

    return messagesLog;
}
