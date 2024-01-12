#include "Monitor.hpp"

#include <string>
#include <functional>
#include <algorithm>
#include "TaskManager.hpp"

void Monitor::Init()
{
    try
    {
        TaskConfig monitorTaskConfig = TaskConfig
        (
            "Monitor",
            [this]() { MainFunction(); },
            2000,
            CORE_1,
            8192,
            9
        );
        scheduler.RequestTask(monitorTaskConfig);
    }
    catch (const std::exception& e)
    {
        logManager.Log(E, "Monitor", "Failed to initialize: %s", e.what());
    }
}

void Monitor::MainFunction()
{
    if(monitorables.size() == 0)
    {
        return;
    }

    std::string monitorLog = "";
    monitorLog += "\n=========BEGIN=========\n";
    monitorLog += "Counter: " + std::to_string(counter) + "\n";

    for (auto monitorable : monitorables)
    {
        monitorLog += monitorable->GetMonitorData();
    }

    monitorLog += "\n==========END==========\n";

    logManager.Log(I, "Monitor", "%s", monitorLog.c_str());

    counter++;
}

void Monitor::Subscribe(IMonitorable* monitorable)
{
    monitorables.push_back(monitorable);
}

void Monitor::Unsubscribe(IMonitorable* monitorable)
{
    auto it = std::find(monitorables.begin(), monitorables.end(), monitorable);
    if (it != monitorables.end())
    {
        monitorables.erase(it);
    }
}
