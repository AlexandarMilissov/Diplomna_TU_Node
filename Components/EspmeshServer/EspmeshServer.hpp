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

    // Mesh message handlers
    void ReceiveMeshNodeConnected(NetIdentifier, std::queue<Payload>);
    // UDP message handlers
    void ReceiveUdpDiscoverRequest(NetIdentifier, std::queue<Payload>);
    // TCP message handlers
    void ReceiveTcpGetNodesRequest();
    void ReceiveTcpGlobalOptionsRequest();

    // Mesh message senders
    void SendMeshGetNodes();
    // UDP message senders
    void SendUdpDiscoverResponse(NetIdentifier);
    // TCP message senders
    void SendTcpNodeConnected(NetIdentifier, std::stack<Payload>);
    void SendTcpGlobalOptionsResponse(std::stack<Payload>);

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