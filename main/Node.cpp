#include "Common.hpp"

#include "TaskManager.hpp"
#include "EspnowManager.hpp"
#include "LogManager.hpp"
#include "EspnowDriver.hpp"
#include "EspmeshDriver.hpp"
#include "EspmeshManager.hpp"
#include "EspmeshServer.hpp"
#include "WifiDriver.hpp"
#include "NvsManager.hpp"
#include "CpuMonitor.hpp"
#include "RamMonitor.hpp"
#include "Monitor.hpp"
#include "PortDriver.hpp"

#include <vector>

std::vector<IComponent*>* components;
extern "C" void app_main(void)
{
    components = new std::vector<IComponent*>();

    auto logManager     = new LogManager    ();
    auto cpuMonitor     = new CpuMonitor    ();
    auto ramMonitor     = new RamMonitor    ();
    auto wifiDriver     = new WifiDriver    ();
    auto nvsManager     = new NvsManager    (*logManager);
    auto taskManager    = new TaskManager   (*logManager);
    auto monitor        = new Monitor       (*logManager, *taskManager);
    auto espnowDriver   = new EspnowDriver  (*logManager, *taskManager);
    auto portDriver     = new PortDriver    (*logManager, *taskManager, *nvsManager);
    auto espmeshDriver  = new EspmeshDriver (*logManager, *taskManager, *nvsManager);
    auto espmeshServer  = new EspmeshServer (*logManager, *taskManager, *nvsManager, *espmeshDriver, *portDriver);
    auto espnowManager  = new EspnowManager (*logManager, *taskManager, *espnowDriver);
    auto espmeshManager = new EspmeshManager(*logManager, *taskManager, *espmeshDriver, *espnowManager);

    // monitor->Subscribe(cpuMonitor);
    // monitor->Subscribe(ramMonitor);
    // monitor->Subscribe(espnowManager);
    // monitor->Subscribe(espmeshManager);
    // monitor->Subscribe(espmeshServer);

    wifiDriver->Subscribe(*portDriver);

    espnowDriver->Subscribe(*espnowManager);

    espmeshDriver->Subscribe(*espmeshManager);
    espmeshDriver->Subscribe(*espmeshServer);

    portDriver->Subscribe(*espmeshServer);

    components->push_back(logManager);
    components->push_back(nvsManager);
    components->push_back(taskManager);
    components->push_back(cpuMonitor);
    components->push_back(ramMonitor);
    components->push_back(monitor);
    components->push_back(wifiDriver);
    components->push_back(espnowDriver);
    components->push_back(espmeshDriver);
    components->push_back(espnowManager);
    components->push_back(espmeshManager);
    components->push_back(portDriver);

    for(IComponent* component : *components)
    {
        component->Init();
    }

    logManager->Log(E, "System", "This is %s. End of Init.\n", nvsManager->GetVar<std::string>("Info", "name", "Default").c_str());
}
