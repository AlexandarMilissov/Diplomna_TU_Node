#include "Common.hpp"

#include "TaskManager.hpp"
#include "EspnowManager.hpp"
#include "LogManager.hpp"
#include "EspnowDriver.hpp"
#include "EspmeshDriver.hpp"
#include "EspmeshManager.hpp"
#include "WifiManager.hpp"
#include "NvsManager.hpp"
#include "Monitor.hpp"

#include <vector>

std::vector<IComponent*>* components;
extern "C" void app_main(void)
{
    components = new std::vector<IComponent*>();

    LogManager* logManager = new LogManager();
    NvsManager* nvsManager = new NvsManager(*logManager);
    TaskManager* taskManager = new TaskManager(*logManager);
    Monitor* monitor = new Monitor(*logManager, *taskManager);
    WifiManager* wifiManager = new WifiManager();
    EspnowDriver* espnowDriver = new EspnowDriver(*logManager);
    EspmeshDriver* espmeshDriver = new EspmeshDriver(*nvsManager, *logManager, *taskManager);
    EspnowManager* espnowManager = new EspnowManager(*espnowDriver, *logManager, *taskManager);
    EspmeshManager* espmeshManager = new EspmeshManager(*espmeshDriver, *espnowManager, *logManager, *taskManager);

    components->push_back(logManager);
    components->push_back(nvsManager);
    components->push_back(taskManager);
    // components->push_back(monitor);
    components->push_back(wifiManager);
    components->push_back(espnowDriver);
    components->push_back(espmeshDriver);
    components->push_back(espnowManager);
    components->push_back(espmeshManager);

    for(IComponent* component : *components)
    {
        component->Init();
    }

    monitor->Subscribe(monitor);
    monitor->Subscribe(espnowManager);
    monitor->Subscribe(espmeshManager);

    logManager->Log(E, "System", "This is %s. End of Init.\n", nvsManager->GetVar<std::string>("Info", "name", "Default").c_str());
}
