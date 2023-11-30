#include "Common.h"
#include "EspnowDriver.h"
#include "TaskManager.h"
#include "EspnowManager_Internal.hpp"
#include "EspnowManager_Tasks.hpp"
#include "EspnowManager_Interface.hpp"
#include "EspnowManager_Communication.hpp"
#include "OpenSeries.hpp"
#include <string>

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
#include "Monitor.hpp"
const char* Encapsulation_Log();
#endif

void EspnowManager_Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    Peers = {};
    interruptReceivedMessages = {};

    LogWrapper_SetMinimalLevel("EspnowManager", I);

    EspnowDriver_Init(ReceiveMessage);

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
    Monitor_SubscribeFunction(&Encapsulation_Log);
#endif

    espnowManagerInternalState = INIT;

#if 1
    EspnowManager_ActivateNetwork();
#endif
}

void EspnowManager_MainFunctionUpdatePeers(const void* pvParameters)
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
            LogWrapper_Log(W, "EspnowManager", "Peer has disconnected.");
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

void EspnowManager_MainFunction_Send_Cyclic_KeepAlive(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != espnowManagerInternalState)
    {
        return;
    }

    // Send keep alive messages
    RSSI_Message_Keep_Alive::StaticSend();

    // Send needed subscription requests
    for(auto& peer : Peers)
    {
        peer->SendSubscriptionRequest();
    }
}

void EspnowManager_MainFunction_Send_Cyclic_Calculation(const void* pvParameters)
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
        RequestTask(&task_cfg);
    }
}

void EspnowManager_MainFunction_HandleReceivedMessages(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != espnowManagerInternalState)
    {
        return;
    }

    HandleReceivedMessages();
}

void EspnowManager_SendCalculationSeries(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    RSSI_Message_Calculation::StaticSend();
}

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
const char* Encapsulation_Log()
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
