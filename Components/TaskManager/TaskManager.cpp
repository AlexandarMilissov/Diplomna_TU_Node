#include "Common.hpp"
#include "TaskManager.hpp"
#include "TaskManager_cfg.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include <string>
#include "esp_heap_caps.h"
#include "Monitor.hpp"
#include "NvsManager.hpp"

void TaskManager::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

    LogManager::SetMinimalLevel("EspnowPeer", I);
    LogManager::SetMinimalLevel("TaskManager", W);

    for (size_t i = 0; i < Init_cfg_size; i++)
    {
        init_cfg[i].InitFunction(NULL);
    }

    for (size_t i = 0; i < Task_cfg_size; i++)
    {
        RequestTask(task_cfg + i);
    }

    LogManager::Log(E, "TaskManager", " This is %s. Init success.\n", NvsManager::GetVar<std::string>("name").c_str());
}

/**
 * @brief Executes the task specified by the given configuration.
 *
 * This function is responsible for executing the task based on the provided configuration.
 * It handles task repetition, timing, and termination conditions.
 *
 * @param in_config_ptr A pointer to the configuration structure for the task.
 */
void TaskManager::Task(void* in_config_ptr)
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

            LogManager::Log(E, "TaskManager", "Task '%s' failed. Main function has become NULL. Terminating.", pcTaskGetName(xTaskGetCurrentTaskHandle() ));
            shouldExit = true;
            continue;
        }
        cfg->MainFunction(cfg->mainFunctionParams);
        time = esp_timer_get_time() - time;

        esp_task_wdt_reset();
        if((cfg->period * 1000) >= time)
        {
            LogManager::Log(V, "TaskManager", "%s executed on time by %lld/%lld us\n", task_name_table[cfg->namePointer], time, (uint64)cfg->period * 1000);
            TaskSleepMiliSeconds(cfg->period - (time / 1000));
        }
        else
        {
            LogManager::Log(W, "TaskManager", "%s took to long to execute by %lld/%lld us\n", task_name_table[cfg->namePointer], time, (uint64)cfg->period * 1000);
        }
    }
    if(NULL != cfg->OnComplete)
    {
        cfg->OnComplete(cfg->onCompleteParams);
    }

    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}

TaskHandle_t* TaskManager::RequestTask(Task_cfg_struct* config)
{
    if(!IsTaskCfgValid(*config))
    {
        LogManager::Log(E, "TaskManager", "Invalidly configured task: %s", task_name_table[config->namePointer]);
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
        LogManager::Log(E, "TaskManager", "Failed to create task: %s", task_name_table[config->namePointer]);
    }
    return taskHandle;
}

bool TaskManager::IsTaskCfgValid(Task_cfg_struct config)
{
    if(NULL == config.MainFunction)
    {
        return false;
    }
    if(portTICK_PERIOD_MS > config.period)
    {
        return false;
    }
    if(wdtTimeout <= config.period)
    {
        return false;
    }

    return true;
}
