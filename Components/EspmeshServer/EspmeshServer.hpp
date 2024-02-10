#ifndef ESPMESHSERVER_HPP_
#define ESPMESHSERVER_HPP_

#include "Common.hpp"
#include "BaseComponent.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "IMonitorable.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "NvsManager.hpp"

#include "lwip/sockets.h"

class EspmeshServer : public BaseComponent, public IMessageReceiver, public IMonitorable
{
private:
    LogManager& logManager;
    IScheduler& taskManager;
    NvsManager& nvsManager;
    IMessageSender& innerNetwork;
    IMessageSender& outerNetwork;

    bool isServer = false;

    void ReceiveRootUpdated(std::queue<Payload>);
    void ReceiveUdpDiscoverRequest(NetIdentifier, std::queue<Payload>);
    void ReceiveTcpGetNodesRequest();
    void ReceiveMeshNodeConnected(NetIdentifier, std::queue<Payload>);

    void SendUdpDiscoverResponse(NetIdentifier);

    void StartServer();
    void StopServer();
public:
    EspmeshServer(
        LogManager& logManager,
        IScheduler& taskManager,
        NvsManager& nvsManager,
        IMessageSender& innerNetwork,
        IMessageSender& outerNetwork
        ) :
        logManager(logManager),
        taskManager(taskManager),
        nvsManager(nvsManager),
        innerNetwork(innerNetwork),
        outerNetwork(outerNetwork)
    {}

    void Init();

    void Receive(const NetIdentifier, const std::queue<Payload>);

    std::string GetMonitorData();
};


#endif // ESPMESHSERVER_HPP_