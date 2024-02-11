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

extern "C" void app_main(void)
{

    auto logManager     = new LogManager    ();
    auto cpuMonitor     = new CpuMonitor    ();
    auto ramMonitor     = new RamMonitor    ();
    auto nvsManager     = new NvsManager    (*logManager);
    auto wifiDriver     = new WifiDriver    ();
    auto taskManager    = new TaskManager   (*logManager);
    auto monitor        = new Monitor       (*logManager, *taskManager);
    auto espnowDriver   = new EspnowDriver  (*logManager, *taskManager);
    auto portDriver     = new PortDriver    (*logManager, *taskManager, *nvsManager);
    auto espmeshDriver  = new EspmeshDriver (*logManager, *taskManager, *nvsManager);
    auto espmeshServer  = new EspmeshServer (*logManager, *taskManager, *nvsManager, *espmeshDriver, *portDriver);
    auto espnowManager  = new EspnowManager (*logManager, *taskManager, *espnowDriver);
    auto espmeshManager = new EspmeshManager(*logManager, *taskManager, *nvsManager, *espmeshDriver, *espnowManager);

    DUMMY_STATEMENT(cpuMonitor);
    DUMMY_STATEMENT(ramMonitor);
    DUMMY_STATEMENT(monitor);

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


    for(BaseComponent* component : BaseComponent::GetComponents())
    {
        component->Init();
    }

    logManager->Log(E, "System", "This is %s. End of Init.\n", nvsManager->GetVar<std::string>("Info", "name", "Default").c_str());
}
