#include "RamMonitor.hpp"

#include "esp_heap_caps.h"

void RamMonitor::Init()
{
    maximum_heap_regions = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
}

std::string RamMonitor::GetMonitorData()
{
    std::string memoryLog = "";
    size_t free_heap_regions = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    memoryLog += "Maximum memory: " + std::to_string(maximum_heap_regions) + "B\n";
    memoryLog += "Used    memory: " + std::to_string(maximum_heap_regions - free_heap_regions) + "B\n";
    memoryLog += "Free    memory: " + std::to_string(free_heap_regions) + "B\n";
    memoryLog += "Percentage free: " + std::to_string((float)free_heap_regions * 100 / (float)maximum_heap_regions) + "%\n";

    return memoryLog;
}
