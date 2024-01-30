#ifndef WIFIDRIVER_HPP
#define WIFIDRIVER_HPP

#include "Common.hpp"
#include "IComponent.hpp"
#include "esp_mac.h"
#include "MacAddress.hpp"
#include "NvsManager.hpp"

/**
 * @brief The WifiDriver class handles the initialization of the WiFi module.
 */
class WifiDriver : public IComponent
{
private:
    static MacAddress broadcastMac;
    static MacAddress myStaMac;
    static MacAddress myApMac;
public:

    WifiDriver();
    /**
     * @brief Initializes the WiFi module.
     *
     * @param data A pointer to the initialization data.
     */
    void Init();

    static MacAddress GetMyStaMac();
    static MacAddress GetMyApMac();
    static MacAddress GetBroadcastMac();
};

#endif
