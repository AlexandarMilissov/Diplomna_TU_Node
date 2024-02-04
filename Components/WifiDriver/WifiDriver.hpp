#ifndef WIFIDRIVER_HPP
#define WIFIDRIVER_HPP

#include "Common.hpp"
#include "IComponent.hpp"
#include "esp_mac.h"
#include "MacAddress.hpp"
#include "NvsManager.hpp"
#include "IMessageReceiver.hpp"

/**
 * @brief The WifiDriver class handles the initialization of the WiFi module.
 */
class WifiDriver : public IComponent
{
private:
    static MacAddress broadcastMac;
    static MacAddress myStaMac;
    static MacAddress myApMac;

    static std::vector<WifiDriver*> drivers;

    std::vector<IMessageReceiver*> upperLayerMessageables;
public:
    static MacAddress GetMyStaMac();
    static MacAddress GetMyApMac();
    static MacAddress GetBroadcastMac();

    WifiDriver();
    ~WifiDriver();

    void Init();

    void Subscribe(IMessageReceiver& messageable);

    static void ReceiveIpEvent(void*, esp_event_base_t, sint32, void*);
    void DistributeIpEvents(void*, esp_event_base_t, sint32, void*);

    void ReceiveIpEventStaGotIp(void*, esp_event_base_t, sint32, void*);
    void ReceiveIpEventStaLostIp(void*, esp_event_base_t, sint32, void*);
};

#endif
