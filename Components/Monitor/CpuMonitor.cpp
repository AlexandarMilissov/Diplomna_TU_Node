#include "CpuMonitor.hpp"
#include "TaskManager.hpp"

CpuMonitor::CpuMonitor(/* args */)
{
}

CpuMonitor::~CpuMonitor()
{
}

void CpuMonitor::Init()
{
}

std::string CpuMonitor::GetMonitorData()
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
    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

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

            // Using std::sort to sort the array of pointers
            std::sort(pxTaskStatusArray, pxTaskStatusArray + uxArraySize,
                      [](const TaskStatus_t &a, const TaskStatus_t &b)
                      {
                          return (a.ulRunTimeCounter < b.ulRunTimeCounter);
                      });

            // For each populated position in the pxTaskStatusArray array
            for (UBaseType_t i = 0; i < uxArraySize; i++)
            {
                if (pxTaskStatusArray[i].xCoreID == CORE_0)
                {
                    ulStatsAsPercentage = ((float)(pxTaskStatusArray[i].ulRunTimeCounter)) / ((float)ulTotalRunTime);
                    cpuLogCore0 += "    ";
                    if (ulStatsAsPercentage < 10)
                    {
                        cpuLogCore0 += "0";
                    }
                    cpuLogCore0 += std::to_string(ulStatsAsPercentage);
                    cpuLogCore0 += "% : ";
                    cpuLogCore0 += pxTaskStatusArray[i].pcTaskName;
                    cpuLogCore0 += "\n";
                }
                else if (pxTaskStatusArray[i].xCoreID == CORE_1)
                {
                    ulStatsAsPercentage = ((float)(pxTaskStatusArray[i].ulRunTimeCounter)) / ((float)ulTotalRunTime);
                    cpuLogCore1 += "    ";
                    if (ulStatsAsPercentage < 10)
                    {
                        cpuLogCore1 += "0";
                    }
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
    return cpuLog;
}
