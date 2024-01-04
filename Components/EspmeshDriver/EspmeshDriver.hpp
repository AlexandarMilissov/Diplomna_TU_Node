#ifndef ESPMESHDRIVER_HPP_
#define ESPMESHDRIVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "NvsManager.hpp"

class EspmeshDriver : public IComponent, public IMessageSender
{
private:
    NvsManager& nvsManager;
    LogManager& logManager;
    IScheduler& scheduler;

    static std::vector<EspmeshDriver*> drivers;

    std::vector<IMessageReceiver*> upperLayerMessageables;

    static void ReceiveMeshEvent(void*, esp_event_base_t, sint32, void*);
    void DistributeMeshEvents(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventRootAddress(void*, esp_event_base_t, sint32, void*);
    void ReceiveMeshEventChildDisconnected(void*, esp_event_base_t, sint32, void*);

    void ConnectRouterless();
    void ConnectRouter();

    void Receive();
public:
    EspmeshDriver(
        NvsManager& nvsManager,
        LogManager& logManager,
        IScheduler& scheduler
    );

    void Init();

    void Subscribe(IMessageReceiver& component);
    void Send(const MacAddress, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
};


#endif // ESPMESHDRIVER_HPP_
