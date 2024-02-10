#ifndef PORTDRIVER_HPP_
#define PORTDRIVER_HPP_

#include "Common.hpp"
#include "BaseComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "NvsManager.hpp"
#include "NetSocket.hpp"

#include "lwip/sockets.h"
#include <map>

typedef sint32 NetSocketDescriptor;

class PortDriver : public BaseComponent, public IMessageSender, public IMessageReceiver
{
private:
    static const uint16 PORT = 9876;

    bool isUdpRunning = false;
    bool isTcpRunning = false;

    NetSocketDescriptor udpSocket = -1;
    NetSocketDescriptor tcpSocket = -1;

    Spinlock tcpClientsSpinlock = Spinlock_Init;
    std::map<NetSocket, NetSocketDescriptor> tcpClients;

    LogManager& logManager;
    IScheduler& scheduler;
    NvsManager& nvsManager;

    std::vector<IMessageReceiver*> upperLayerMessageables;

    void GotIp(const uint32 ip);
    void LostIp();

    void StartUdp();
    void StartTcp();

    void StopUdp();
    void StopTcp();

    void ReceiveUdp();
    void ReceiveTcp();

    void SendUdp(const NetIdentifier, const std::stack<Payload>);
    void SendTcp(const NetSocketDescriptor, const std::stack<Payload>);

    void AcceptTcp();

public:
    PortDriver(LogManager&, IScheduler&, NvsManager&);
    ~PortDriver();

    void Send(const NetIdentifier, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
    void Receive(const NetIdentifier, std::queue<Payload>);
    void Subscribe(IMessageReceiver&);
};

#endif // PORTDRIVER_HPP_
