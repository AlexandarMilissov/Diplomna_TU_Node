#include "Common.h"
#include "EspnowManager.h"
#include "EspnowDriver.h"
#include "TaskManager.h"
#include "To_CPP_Encapsulation.hpp"
#include <stdatomic.h>

_Atomic State state = NO_INIT;
_Atomic uint16 calculationSubscribers = 0;

void EspnowManager_Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    esp_log_level_set("EspnowManager", ESP_LOG_INFO);

    EspnowDriver_Init(MessageReceive);

    To_CPP_Encapsulation_Init(NULL);

    state = INIT;

#if 1
    EspnowManager_ActivateNetwork();
#endif
}

void EspnowManager_MainFunctionUpdatePeers(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    UpdatePeers();
}

void EspnowManager_MainFunction_Send_Cyclic_KeepAlive(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != state)
    {
        return;
    }
    Send_Cyclic_Msg();
}

void EspnowManager_MainFunction_Send_Cyclic_Calculation(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != state)
    {
        return;
    }
    if(0 < calculationSubscribers)
    {
        static Task_cfg_struct task_cfg = EspnowManager_SendCalculationFunction_Config;
        task_cfg.repetition = (uint16)GetSeriesRepetitions();
        RequestTask(&task_cfg);
    }
}

void EspnowManager_MainFunction_HandleReceivedMessages(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    if(RUN != state)
    {
        return;
    }
    HandleReceivedMessages();
}

void EspnowManager_ActivateNetwork()
{
    state = RUN;
}

void EspnowManager_DeactivateNetwork()
{
    state = INIT;
}

void AddCalculationSubscriber()
{
    calculationSubscribers++;
}

void RemoveCalculationSubscriber()
{
    calculationSubscribers--;
}