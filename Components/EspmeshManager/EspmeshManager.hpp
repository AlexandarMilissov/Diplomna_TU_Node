#ifndef ESPMESHMANAGER_HPP_
#define ESPMESHMANAGER_HPP_

#include "Common.hpp"

#include "IScheduler.hpp"
#include "IComponent.hpp"
#include "IEspnowController.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
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

class EspmeshManager : public IComponent, public IMessageReceiver, public IMonitorable
{
private:
    LogManager& logManager;
    IScheduler& taskManager;
    IMessageSender& lowerLayer;
    IEspnowController& espnowController;

    std::atomic<EspmeshManagerState> internalState = MESH_NO_INIT;
    MacAddress rootAddress;

    void MainFunction();

    void MainFunctionSendKeepAlive();
public:
    EspmeshManager(
        LogManager& logManager,
        IScheduler& taskManager,
        IMessageSender& lowerLayer,
        IEspnowController& espnowController
        ) :
        logManager(logManager),
        taskManager(taskManager),
        lowerLayer(lowerLayer),
        espnowController(espnowController)
        {}
    ~EspmeshManager() {}
    void Init();

    void Receive(const MacAddress, std::queue<Payload>);

    std::string GetMonitorData();
};

#endif // ESPMESHMANAGER_HPP_
