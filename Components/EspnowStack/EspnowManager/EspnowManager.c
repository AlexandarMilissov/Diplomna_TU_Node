#include "Common.h"
#include "EspnowManager.h"
#include "EspnowDriver.h"
#include "TaskManager.h"
#include "To_CPP_Encapsulation.hpp"

State state = NO_INIT;
uint8 myID = 2;
uint16 calculationSubscibers = 0;

void EspnowManager_Init(void* pvParameters)
{
    esp_log_level_set("EspnowManager", ESP_LOG_INFO);
    
    EspnowDriver_Init(MessageReceive);

    To_CPP_Encapsulation_Init(NULL);

    state = INIT;

    EspnowManager_ActivateNetwork();
}

void EspnowManager_MainFunction(void* pvParameters)
{
    if(RUN != state)
    {
        return;
    }
    HandleReceivedMessages();

    if(0 < calculationSubscibers)
    {
        Task_cfg_struct task_cfg = {
            .core = CORE_1,
            .finite = true,
            .repetition = GetSeriersRepetitions(),
            .MainFunction = SeriesSend,
            .name = "Series Task",
            .OnComplete = NULL,
            .onCompleteParams = NULL,
            .period = 3,
            .priority = 200,
            .stack_size = 8192,
        };
        RequestTask(task_cfg);
    }
    Send_Cyclic_Msg();

    UpdateSeries();
}
void EspnowManager_ActivateNetwork()
{
    state = RUN;
}

void EspnowManager_DeactivateNetwork()
{
    state = INIT;
}

void AddCalculationSubsciber()
{
    calculationSubscibers++;
    ESP_LOGI("EspnowManager","Subscibers: %d\n", calculationSubscibers);
}

void RemoveCalculationSubsciber()
{
    calculationSubscibers--;
    ESP_LOGI("EspnowManager","Subscibers: %d\n", calculationSubscibers);
}