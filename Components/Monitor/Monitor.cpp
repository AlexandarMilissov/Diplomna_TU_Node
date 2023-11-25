#include "Monitor.hpp"
#include "To_CPP_Encapsulation.hpp"

#include <vector>
#include <string>
#include <functional>

std::vector<LogFunctionSignature> logDelegate;

size_t maximum_heap_regions;
size_t free_heap_regions;

const char* MonitorMemory();

void Monitor_Init(const void*)
{
    maximum_heap_regions = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MEMORY_MONITOR
    Monitor_SubscribeLog(&MonitorMemory);
#endif
}

void Monitor_MainFunction(const void*)
{
    static uint16 counter = 0;
    static std::string monitorLog = "";

    monitorLog += "\n=========BEGIN=========\n";
    monitorLog += "Counter: "+ std::to_string(counter) + "\n";

    for (const auto& func : logDelegate) {
        monitorLog += func();
    }

    monitorLog += "\n==========END==========\n";


    ESP_LOGI("Monitor", "%s", monitorLog.c_str());

    monitorLog = "";
    counter++;
}

void Monitor_SubscribeLog(LogFunctionSignature logger)
{
    logDelegate.push_back(logger);
}

void Monitor_UnsubscribeLog(LogFunctionSignature logger)
{
    auto it = std::find(logDelegate.begin(), logDelegate.end(), logger);
    if (it != logDelegate.end()) {
        logDelegate.erase(it);
    }
}

const char* MonitorMemory()
{
    static std::string memoryLog;
    free_heap_regions = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    memoryLog = "";
    memoryLog += "Used memory " + std::to_string(maximum_heap_regions - free_heap_regions);
    memoryLog += "B out of "    + std::to_string(maximum_heap_regions);
    memoryLog += "B maximum ("  + std::to_string((double)free_heap_regions*100/(double)maximum_heap_regions);
    memoryLog += " free)";
    memoryLog += "\n\0";

    return memoryLog.c_str();
}
