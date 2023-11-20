#include "Monitor.hpp"
#include "To_CPP_Encapsulation.hpp"
#include <map>
#include <string>

using namespace std;

typedef struct TaskMonitor
{
    const uint16 period;
    uint16 entries;
    uint64 avrg_time;
    uint64 last_time;
}TaskMonitor;

size_t maximum_heap_regions;
size_t free_heap_regions;
map<uint8, TaskMonitor*> taskAnalysis;

void Monitor_Init(const void*)
{
    maximum_heap_regions = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
}

void Monitor_MainFunction(const void*)
{
    free_heap_regions = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI("Monitor", "=====================");

    ESP_LOGI("Monitor", "Used memory %dB out of %dB maximum (%f%% free)",
            maximum_heap_regions - free_heap_regions ,maximum_heap_regions , (double)free_heap_regions*100/(double)maximum_heap_regions);

    EncapsulationMonitor();
    ESP_LOGI("Monitor", "=====================");
}

void MonitorTaskInit(uint8 name_id, const uint16 period)
{
    if (taskAnalysis.count(name_id))
    {
        taskAnalysis[name_id]->entries++;
    }
    else 
    {
        
    }
}
