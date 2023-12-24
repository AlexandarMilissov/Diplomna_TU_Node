#include "Common.hpp"
#include "TaskManager.hpp"
#include "Payload.hpp"
#include "OpenSeries.hpp"
#include <string>

#include "EspnowManager.hpp"

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
    interruptReceivedMessages = {};

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

    internalState = INIT;
}

void EspnowManager::MainFunctionUpdatePeers()
{
    size_t count = 0;
    std::vector<size_t> toRemove;

    for(auto peer:Peers)
    {
        if(peer->IsAlive())
        {
            peer->Refresh();
        }
        else
        {
            logManager.Log(W, "EspnowManager", "EspnowPeer has disconnected.");
            delete peer;
            toRemove.push_back(count);
        }
        count++;
    }
    for(size_t i = 0; i < toRemove.size(); i++)
    {
        Peers.erase(Peers.begin() + toRemove.at(i));
    }
}

void EspnowManager::MainFunctionSendCyclicKeepAlive()
{
    if(RUN != internalState)
    {
        return;
    }

    // Send keep alive messages
    EspnowMessageKeepAlive keepAlive;
    Payload header(broadcast_mac, sizeof(broadcast_mac));
    Send(header, keepAlive.GetPayload());

    // Send needed subscription requests
    for(auto& peer : Peers)
    {
        peer->SendSubscriptionRequest();
    }
}

void EspnowManager::MainFunctionSeriesBegin()
{
    if(RUN != internalState)
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
    if(RUN != internalState)
    {
        return;
    }

    HandleReceivedMessages();
}

void EspnowManager::SendCalculationSeries()
{
    EspnowMessageCalculation calculation;

    Payload header(broadcast_mac, sizeof(broadcast_mac));
    Send(header, calculation.GetPayload());
}

std::string EspnowManager::GetMonitorData()
{
    static std::string messagesLog;
    size_t operations;
    uint64 localHandledMessagesCounter;
    uint64 localReceivedMessagesCounter;

    Enter_Critical_Spinlock(InterruptReceivedMessagesSpinlock);
    operations = interruptReceivedMessages.size();
    localHandledMessagesCounter = handledMessagesCounter;
    localReceivedMessagesCounter = receivedMessagesCounter;
    Exit_Critical_Spinlock(InterruptReceivedMessagesSpinlock);

    messagesLog = "\n";
    messagesLog += "[" + std::to_string(operations                  ) + "] queued messages\n"   ;
    messagesLog += "[" + std::to_string(localHandledMessagesCounter ) + "] handled messages\n"  ;
    messagesLog += "[" + std::to_string(localReceivedMessagesCounter) + "] received messages\n" ;

    for(auto& peer : Peers)
    {
        messagesLog += peer->GetMonitorData();
    }

    return messagesLog;
}
