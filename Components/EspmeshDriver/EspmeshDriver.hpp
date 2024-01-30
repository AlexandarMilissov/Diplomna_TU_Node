#ifndef ESPMESHDRIVER_HPP_
#define ESPMESHDRIVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "NvsManager.hpp"
#include "MacAddress.hpp"

class EspmeshDriver : public IComponent, public IMessageSender
{
private:
    LogManager& logManager;
    IScheduler& scheduler;
    NvsManager& nvsManager;

    MacAddress myAddress;
    MacAddress parentAddress;
    MacAddress rootAddress;
    MacAddress broadcastAddress;
    Ipv4Address* myIp = NULL;
    bool* isRoot = NULL;

    size_t receiveBufferLength;
    uint8* receiveBuffer = NULL;

    static std::vector<EspmeshDriver*> drivers;

    std::vector<IMessageReceiver*> upperLayerMessageables;

    static void ReceiveWifiEvent(void*, esp_event_base_t, sint32, void*);
    void DistributeMeshEvents(void*, esp_event_base_t, sint32, void*);
    void DistributeIpEvents(void*, esp_event_base_t, sint32, void*);

    void ReceiveMeshEventRootAddress(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventChildDisconnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventParentConnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventParentDisconnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventToDsState(void*, esp_event_base_t, sint32, void*);

    void ReceiveIpEventStaGotIp(void*, esp_event_base_t, sint32, void*);
    void ReceiveIpEventStaLostIp(void*, esp_event_base_t, sint32, void*);

    void NotifyUpperLayerRootIsSet();

    void ConnectRouterless();
    void ConnectRouter();

    void Receive();
public:
    EspmeshDriver(
        LogManager& logManager,
        IScheduler& scheduler,
        NvsManager& nvsManager
    );

    ~EspmeshDriver();

    void Init();

    void Subscribe(IMessageReceiver& component);
    void Send(const MacAddress, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
};


#endif // ESPMESHDRIVER_HPP_
