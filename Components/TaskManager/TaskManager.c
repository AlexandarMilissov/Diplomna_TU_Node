#include "Common.h"
#include "TaskManager.h"
#include "TaskManager_cfg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "nvs_flash.h"
void Task(void*);
bool IsTaskCfgValid(Task_cfg_struct);

#define WATCHDOG_RESET_TIME_IN_SECONDS 5
#define WATCHDOG_RESET_MAX_TASK_TIME 0.9 * WATCHDOG_RESET_TIME_IN_SECONDS

void TaskManager_Init(void)
{
    nvs_flash_init();
    esp_task_wdt_init(WATCHDOG_RESET_TIME_IN_SECONDS, true);

    for (size_t i = 0; i < Init_cfg_size; i++)
    {
        init_cfg[i].InitFunction(NULL);
    }

    for (size_t i = 0; i < Task_cfg_size; i++)
    {
        RequestTask(task_cfg[i]);
    }
    printf("init success\n");
}

void Task(void* config)
{
    Task_cfg_struct cfg = *((Task_cfg_struct*)config);
    uint64 time = 0;
    esp_task_wdt_add(NULL);

    for(;;)
    {
        if(cfg.finite)
        {
            if (0 == cfg.repetition)
            {
                esp_task_wdt_reset();
                break;
            }
            cfg.repetition--;
        }

        time = esp_timer_get_time();
        cfg.MainFunction(cfg.mainFunctionParams);
        time = esp_timer_get_time() - time;

        esp_task_wdt_reset();
        if((cfg.period * 1000) >= time)
        {
            TaskSleepMiliSeconds(cfg.period - (time / 1000));
            if(cfg.finite == true)
            {
                printf("%s executed on time by %lld/%lldus\n", cfg.name, time, (uint64)cfg.period * 1000);
            }
        }
        else
        {
            printf("%s took to long to execute by %lld/%lldus\n", cfg.name, time, (uint64)cfg.period * 1000);
            // task took more time to complete than it was its period
            // TODO: throw error or warning
        }
    }

    if(NULL != cfg.OnComplete)
    {
        cfg.OnComplete(cfg.onCompleteParams);
    }

    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}

TaskHandle_t* RequestTask(Task_cfg_struct config)
{
    if(!IsTaskCfgValid(config))
    {
        return NULL;
    }

    switch (config.core)
    {
    case CORE_0:
    case CORE_1:
        break;
    default:
        config.core %= 2;
        break;
    }
    
    TaskHandle_t* taskHandle = NULL;
    xTaskCreatePinnedToCore(Task, config.name, config.stack_size, &config, config.priority, taskHandle, config.core);
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
    if((WATCHDOG_RESET_MAX_TASK_TIME * 1000) <= config.period)
    {
        return false;
    }

    return true;
}
