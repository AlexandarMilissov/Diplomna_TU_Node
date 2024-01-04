#ifndef ESPMESHMANAGER_HPP_
#define ESPMESHMANAGER_HPP_

#include "Common.hpp"

#include "IScheduler.hpp"
#include "IComponent.hpp"
#include "IEspnowController.hpp"
#include "IMessageable.hpp"
#include "IDriver.hpp"
#include "IMonitorable.hpp"
#include "IScheduler.hpp"
#include "LogManager.hpp"

#include <string>
#include <atomic>

typedef enum
{
    MESH_NO_INIT,    // Before any values are initialized
    MESH_INIT,       // After everything has been initialized, Network is not active
    MESH_ROOT,       // Network is active and this node is the root
    MESH_NON_ROOT    // Network is active and this node is not the root
}EspmeshManagerState;

class EspmeshManager : public IComponent, public IMessageable, public IMonitorable
{
private:
    IDriver& driver;
    IEspnowController& espnowController;
    LogManager& logManager;
    IScheduler& taskManager;

    std::atomic<EspmeshManagerState> internalState = MESH_NO_INIT;
    MacAddress rootAddress;

    void MainFunction();

    void MainFunctionSendKeepAlive();
public:
    EspmeshManager(
        IDriver& driver,
        IEspnowController& espnowController,
        LogManager& logManager,
        IScheduler& taskManager
        ) :
        driver(driver),
        espnowController(espnowController),
        logManager(logManager),
        taskManager(taskManager)
        {}
    ~EspmeshManager() {}
    void Init();

    void Send(const MacAddress, const Payload);
    void SendBroadcast(const Payload);
    void Receive(const MacAddress, const Payload);

    std::string GetMonitorData();
};

#endif // ESPMESHMANAGER_HPP_
