#include "Common.hpp"
#include "EspnowDriver.hpp"
#include "TaskManager.hpp"
#include "EspnowManager_Task_Config.h"
#include "Payload.hpp"
#include "OpenSeries.hpp"
#include <string>

#include "EspnowManager.hpp"

void EspnowManager::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    Peers = {};
    interruptReceivedMessages = {};

    LogManager::SetMinimalLevel("EspnowManager", I);

    EspnowDriver::Subscribe(Receive);

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
    Monitor::SubscribeFunction(Log);
#endif

    espnowManagerInternalState = INIT;

#if 1
    EspnowManager::ActivateNetwork();
#endif

    // Task_cfg_struct updatePeersTask = EspnowManager_MainFunctionUpdatePeers_Config;
    // Task_cfg_struct sendKeepAliveTask = EspnowManager_MainFunction_Send_Cyclic_KeepAlive_Config;
    // Task_cfg_struct sendCalculationTask = EspnowManager_MainFunction_Send_Cyclic_Calculation_Config;
    // Task_cfg_struct HandleReceivedMessages0Task = EspnowManager_MainFunction_HandleReceivedMessages_Core0_Config;
    // Task_cfg_struct HandleReceivedMessages1Task = EspnowManager_MainFunction_HandleReceivedMessages_Core1_Config;
    // TaskManager::RequestTask(&updatePeersTask);
    // TaskManager::RequestTask(&sendKeepAliveTask);
    // TaskManager::RequestTask(&sendCalculationTask);
    // TaskManager::RequestTask(&HandleReceivedMessages0Task);
    // TaskManager::RequestTask(&HandleReceivedMessages1Task);
}

void EspnowManager::MainFunctionUpdatePeers(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

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
            LogManager::Log(W, "EspnowManager", "EspnowPeer has disconnected.");
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

void EspnowManager::MainFunctionSendCyclicKeepAlive(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != espnowManagerInternalState)
    {
        return;
    }

    // Send keep alive messages
    EspnowMessageKeepAlive keepAlive;
    EspnowManager::Send(broadcast_mac, keepAlive.GetPayload());

    // Send needed subscription requests
    for(auto& peer : Peers)
    {
        peer->SendSubscriptionRequest();
    }
}

void EspnowManager::MainFunctionSendCyclicCalculation(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != espnowManagerInternalState)
    {
        return;
    }
    if(0 < calculationSubscribers)
    {
        static Task_cfg_struct task_cfg = EspnowManager_SendCalculationFunction_Config;
        task_cfg.repetition = (uint16)OpenSeries::GetNumberOfMessagesPerSeries();
        TaskManager::RequestTask(&task_cfg);
    }
}

void EspnowManager::MainFunctionHandleReceivedMessages(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != espnowManagerInternalState)
    {
        return;
    }

    HandleReceivedMessages();
}

void EspnowManager::SendCalculationSeries(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    EspnowMessageCalculation calculation;
    Send(broadcast_mac, calculation.GetPayload());
}

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
const char* EspnowManager::Log()
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

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR
    for(auto& peer : Peers)
    {
        messagesLog += peer->Log();
    }
#endif

    return messagesLog.c_str();
}
#endif