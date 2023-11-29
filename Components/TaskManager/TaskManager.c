#include "Common.h"
#include "TaskManager.h"
#include "TaskManager_cfg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include <string.h>
#include "esp_heap_caps.h"
#include "EspnowManager_Interface.hpp"
#include "Monitor.hpp"
#include "NvsManager.h"

#define TWDT_TIMEOUT_MS 5000

void Task(void*);
bool IsTaskCfgValid(Task_cfg_struct);

void TaskManager_Init(void)
{
    LogWrapper_SetMinimalLevel("Peer", I);
    LogWrapper_SetMinimalLevel("TaskManager", W);

    for (size_t i = 0; i < Init_cfg_size; i++)
    {
        init_cfg[i].InitFunction(NULL);
    }

    for (size_t i = 0; i < Task_cfg_size; i++)
    {
        RequestTask(task_cfg + i);
    }

    LogWrapper_Log(E, "TaskManager", " This is %s. Init success.\n", NvsGetName());
}

void Task(void* in_config_ptr)
{
    uint64 time = 0;

    Task_cfg_struct* cfg = (Task_cfg_struct*)in_config_ptr;
    bool shouldExit = false;

    esp_task_wdt_add(NULL);
    for(;;)
    {
        if(shouldExit)
        {
            break;
        }

        if(cfg->finite)
        {
            if (0 == cfg->repetition)
            {
                esp_task_wdt_reset();
                shouldExit = true;
                continue;
            }
            cfg->repetition--;
        }

        time = esp_timer_get_time();
        if(cfg->MainFunction == NULL)
        {
            esp_task_wdt_delete(NULL);

            LogWrapper_Log(E, "TaskManager", "Task '%s' failed. Main function has become NULL. Terminating.", pcTaskGetName(xTaskGetCurrentTaskHandle() ));
            shouldExit = true;
            continue;
        }
        cfg->MainFunction(cfg->mainFunctionParams);
        time = esp_timer_get_time() - time;

        esp_task_wdt_reset();
        if((cfg->period * 1000) >= time)
        {
            LogWrapper_Log(V, "TaskManager", "%s executed on time by %lld/%lld us\n", task_name_table[cfg->namePointer], time, (uint64)cfg->period * 1000);
            TaskSleepMiliSeconds(cfg->period - (time / 1000));
        }
        else
        {
            LogWrapper_Log(W, "TaskManager", "%s took to long to execute by %lld/%lld us\n", task_name_table[cfg->namePointer], time, (uint64)cfg->period * 1000);
        }
    }

    if(NULL != cfg->OnComplete)
    {
        cfg->OnComplete(cfg->onCompleteParams);
    }

    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}

TaskHandle_t* RequestTask(Task_cfg_struct* config)
{
    if(!IsTaskCfgValid(*config))
    {
        LogWrapper_Log(E, "TaskManager", "Invalidly configured task: %s", task_name_table[config->namePointer]);
        return NULL;
    }

    switch (config->core)
    {
    case CORE_0:
    case CORE_1:
        break;
    default:
        config->core %= 2;
        break;
    }

    TaskHandle_t* taskHandle = NULL;
    if(pdPASS != xTaskCreatePinnedToCore(
        (TaskFunction_t)Task,
        task_name_table[config->namePointer],
        config->stack_size,
        config,
        config->priority,
        taskHandle,
        config->core))
    {
        LogWrapper_Log(E, "TaskManager", "Failed to create task: %s", task_name_table[config->namePointer]);
    }
    return taskHandle;
}

bool IsTaskCfgValid(Task_cfg_struct config)
{
    if(NULL == config.MainFunction)
    {
        return false;
    }
    if(portTICK_PERIOD_MS > config.period)
    {
        return false;
    }
    if(TWDT_TIMEOUT_MS <= config.period)
    {
        return false;
    }

    return true;
}
