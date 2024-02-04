#ifndef PORTDRIVER_HPP_
#define PORTDRIVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "NvsManager.hpp"

#include "lwip/sockets.h"

#include "NetSocket.hpp"

typedef sint32 NetSocketDescriptor;

class PortDriver : public IComponent, public IMessageSender, public IMessageReceiver
{
private:
    static const uint16 PORT = 9876;

    bool isUdpRunning = false;
    bool isTcpRunning = false;

    NetSocketDescriptor udpSocket = -1;

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
    void SendTcp(const NetIdentifier, const std::stack<Payload>);

public:
    PortDriver(LogManager&, IScheduler&, NvsManager&);
    ~PortDriver();

    void Init();
    void Send(const NetIdentifier, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
    void Receive(const NetIdentifier, std::queue<Payload>);
    void Subscribe(IMessageReceiver&);
};

#endif // PORTDRIVER_HPP_
