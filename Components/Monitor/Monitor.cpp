#include "Monitor.hpp"

#include <vector>
#include <string>
#include <functional>
#include "TaskManager.h"

std::vector<LogFunctionSignature> logDelegate;

size_t maximum_heap_regions;
size_t free_heap_regions;

const char *MonitorMemory();
const char *MonitorCPU();

void Monitor_Init(const void *)
{
    maximum_heap_regions = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MEMORY_MONITOR
    Monitor_SubscribeFunction(&MonitorMemory);
#endif
    Monitor_SubscribeFunction(&MonitorCPU);
}

void Monitor_MainFunction(const void *)
{
    static uint16 counter = 0;
    static std::string monitorLog = "";

    monitorLog += "\n=========BEGIN=========\n";
    monitorLog += "Counter: " + std::to_string(counter) + "\n";

    for (const auto &func : logDelegate)
    {
        monitorLog += func();
    }

    monitorLog += "\n==========END==========\n";

    LogWrapper_Log(I, "Monitor", "%s", monitorLog.c_str());

    monitorLog = "";
    counter++;
}

void Monitor_SubscribeFunction(LogFunctionSignature logFunction)
{
    logDelegate.push_back(logFunction);
}

void Monitor_UnsubscribeFunction(LogFunctionSignature logFunction)
{
    auto it = std::find(logDelegate.begin(), logDelegate.end(), logFunction);
    if (it != logDelegate.end())
    {
        logDelegate.erase(it);
    }
}

const char *MonitorMemory()
{
    static std::string memoryLog;
    free_heap_regions = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    memoryLog = "";
    memoryLog += "Used memory " + std::to_string(maximum_heap_regions - free_heap_regions);
    memoryLog += "B out of " + std::to_string(maximum_heap_regions);
    memoryLog += "B maximum (" + std::to_string((double)free_heap_regions * 100 / (double)maximum_heap_regions);
    memoryLog += "% free)";
    memoryLog += "\n\0";

    return memoryLog.c_str();
}

const char *MonitorCPU()
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize;
    uint32 ulTotalRunTime;
    float ulStatsAsPercentage;

    static std::string cpuLog;
    static std::string cpuLogCore0;
    static std::string cpuLogCore1;
    cpuLog = "";
    cpuLogCore0 = "Core 0: \n";
    cpuLogCore1 = "Core 1: \n";

    // Take a snapshot of the number of tasks in case it changes while this
    // function is executing.
    uxArraySize = uxTaskGetNumberOfTasks();

    // Allocate a TaskStatus_t structure for each task.
    pxTaskStatusArray = (TaskStatus_t*)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL)
    {
        // Generate raw status information about each task.
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

        cpuLog += "Total run time: " + std::to_string(ulTotalRunTime) + "\n";
        // For percentage calculations.
        ulTotalRunTime /= 100UL;

        // Avoid divide by zero errors.
        if (ulTotalRunTime > 0)
        {
            // For each populated position in the pxTaskStatusArray array
            for (UBaseType_t i = 0; i < uxArraySize; i++)
            {
                if(pxTaskStatusArray[i].xCoreID == CORE_0)
                {
                    ulStatsAsPercentage = ((float)(pxTaskStatusArray[i].ulRunTimeCounter)) / ((float)ulTotalRunTime);
                    cpuLogCore0 += "    ";
                    cpuLogCore0 += std::to_string(ulStatsAsPercentage);
                    cpuLogCore0 += "% : ";
                    cpuLogCore0 += pxTaskStatusArray[i].pcTaskName;
                    cpuLogCore0 += "\n";
                }
                else if (pxTaskStatusArray[i].xCoreID == CORE_1)
                {
                    ulStatsAsPercentage = ((float)(pxTaskStatusArray[i].ulRunTimeCounter)) / ((float)ulTotalRunTime);
                    cpuLogCore1 += "    ";
                    cpuLogCore1 += std::to_string(ulStatsAsPercentage);
                    cpuLogCore1 += "% : ";
                    cpuLogCore1 += pxTaskStatusArray[i].pcTaskName;
                    cpuLogCore1 += "\n";
                }
            }
            cpuLog += cpuLogCore0;
            cpuLog += cpuLogCore1;
        }

        // The array is no longer needed, free the memory it consumes.
        vPortFree(pxTaskStatusArray);
    }
    return cpuLog.c_str();
}