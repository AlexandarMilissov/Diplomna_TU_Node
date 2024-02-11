#ifndef ESPMESHDRIVER_HPP_
#define ESPMESHDRIVER_HPP_

#include "Common.hpp"
#include "BaseComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "NvsManager.hpp"
#include "MacAddress.hpp"

#include "esp_mesh.h"

class EspmeshDriver : public BaseComponent, public IMessageSender
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

    static void ReceiveMeshEventStatic(void*, esp_event_base_t, sint32, void*);
    void DistributeMeshEvents(void*, esp_event_base_t, sint32, void*);
    void EventTask(std::function<void()>);

    void ReceiveMeshEventRootAddress(mesh_event_root_address_t);
    void ReceiveMeshEventChildDisconnected(mesh_event_child_disconnected_t);
    void ReceiveMeshEventParentConnected();
    void ReceiveMeshEventParentDisconnected();

    void ConnectRouterless();
    void ConnectRouter();

    void Receive();

    uint64 GetMeshIdChecksum();
    Setting GetMeshIdSetting();
    void SetMeshIdSetting(Setting);
public:
    EspmeshDriver(
        LogManager& logManager,
        IScheduler& scheduler,
        NvsManager& nvsManager
    );

    ~EspmeshDriver();

    void Init();
    std::string GetName();
    std::vector<Setting> GetComponentGlobalSettings();

    void Subscribe(IMessageReceiver&);
    void Send(const NetIdentifier, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
};


#endif // ESPMESHDRIVER_HPP_
