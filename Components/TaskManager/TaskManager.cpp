#include "Common.hpp"
#include "TaskManager.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps.h"
#include <string>
#include <cstring>
#include <vector>
#include <tuple>
#include <stdexcept>

void TaskManager::Init()
{
    logManager.SetMinimalLevel("TaskManager", W);
}

void TaskManager::TaskWrapper(void* pvParameters)
{
    std::tuple<TaskManager*, TaskConfig*>* params = (std::tuple<TaskManager*, TaskConfig*>*)(pvParameters);

    TaskManager* taskManager = std::get<0>(*params);
    TaskConfig* configPtr = std::get<1>(*params);
    TaskConfig config = TaskConfig(*configPtr);

    // Delete the tuple that was created in RequestTask
    delete params;
    // Delete the config object that was created in RequestTask
    delete configPtr;

    taskManager->Task(config);
}

void TaskManager::Task(TaskConfig config)
{
    uint64 time = 0;
    bool shouldExit = true;

    uint16 period = config.GetPeriod();

    esp_task_wdt_add(NULL);
    for(;;)
    {
        time = esp_timer_get_time();

        shouldExit = config.Run();

        esp_task_wdt_reset();

        if(shouldExit)
        {
            break;
        }

        time = esp_timer_get_time() - time;

        if((period * 1000) >= time)
        {
            logManager.Log(V, "TaskManager", "%s executed on time by %lld/%lld us\n", config.GetName(), time, (uint64)period * 1000);
            TaskSleepMiliSeconds(period - (time / 1000));
        }
        else
        {
            logManager.Log(W, "TaskManager", "%s took to long to execute by %lld/%lld us\n", config.GetName(), time, (uint64)period * 1000);
        }
    }

    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}

TaskHandle_t* TaskManager::RequestTask(TaskConfig config)
{
    uint8 core = config.GetCore();

    switch (core)
    {
    case CORE_0:
    case CORE_1:
        break;
    default:
        core %= 2;
        break;
    }

    // Declare a pointer that copies the config object
    // It will be deleted in the TaskWrapper
    TaskConfig* configPtr = new TaskConfig(config);

    // Declare a tuple that will be passed to the TaskWrapper
    // It will be deleted in the TaskWrapper
    std::tuple<TaskManager*, TaskConfig*>* taskTuple = new std::tuple<TaskManager*, TaskConfig*>(this, configPtr);

    TaskHandle_t* taskHandle = NULL;
    if(pdPASS != xTaskCreatePinnedToCore(
        (TaskFunction_t)(TaskWrapper),
        config.GetName(),
        config.GetStackSize(),
        (void*)(taskTuple),
        config.GetPriority(),
        taskHandle,
        core))
    {
        logManager.Log(E, "TaskManager", "Failed to create task: %s", config.GetName());
        throw std::runtime_error("Failed to create task.");
    }
    return taskHandle;
}
