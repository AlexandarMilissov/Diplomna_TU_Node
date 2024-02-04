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
    bool isRoot = false;

    size_t receiveBufferLength;
    uint8* receiveBuffer = NULL;

    static std::vector<EspmeshDriver*> drivers;

    std::vector<IMessageReceiver*> upperLayerMessageables;

    static void ReceiveMeshEvent(void*, esp_event_base_t, sint32, void*);
    void DistributeMeshEvents(void*, esp_event_base_t, sint32, void*);

    void ReceiveMeshEventRootAddress(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventChildDisconnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventParentConnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventParentDisconnected(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventToDsState(void*, esp_event_base_t, sint32, void*);

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

    void Subscribe(IMessageReceiver&);
    void Send(const NetIdentifier, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
};


#endif // ESPMESHDRIVER_HPP_
