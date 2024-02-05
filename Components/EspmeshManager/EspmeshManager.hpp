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
#include "NvsManager.hpp"

#include <string>
#include <atomic>

typedef enum
{
    MESH_NO_INIT,    // Before any values are initialized
    MESH_INIT,       // After everything has been initialized, Network is not active
    MESH_RUN,        // Network is active
}EspmeshManagerState;

class EspmeshManager : public IComponent, public IMessageReceiver, public IMonitorable
{
private:
    LogManager& logManager;
    IScheduler& taskManager;
    NvsManager& nvsManager;
    IMessageSender& lowerLayer;
    IEspnowController& espnowController;

    std::atomic<EspmeshManagerState> internalState = MESH_NO_INIT;
    MacAddress rootAddress;
    MacAddress broadcastAddress;

    void MainFunction();

    void ReceiveMeshRootAddress(const NetIdentifier);
    void SendToRootMeshNodeConnected();
public:
    EspmeshManager(
        LogManager& logManager,
        IScheduler& taskManager,
        NvsManager& nvsManager,
        IMessageSender& lowerLayer,
        IEspnowController& espnowController
        ) :
        logManager(logManager),
        taskManager(taskManager),
        nvsManager(nvsManager),
        lowerLayer(lowerLayer),
        espnowController(espnowController)
        {}
    ~EspmeshManager() {}
    void Init();

    void Receive(const NetIdentifier, std::queue<Payload>);

    std::string GetMonitorData();
};

#endif // ESPMESHMANAGER_HPP_
