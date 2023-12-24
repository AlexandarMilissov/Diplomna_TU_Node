#ifndef WIFIMANAGER_HPP
#define WIFIMANAGER_HPP

#include "Common.hpp"
#include "IComponent.hpp"
#include "esp_mac.h"

/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_WIFI_AP_ENABLED
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#define ESPNOW_MAC       ESP_MAC_WIFI_SOFTAP
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#define ESPNOW_MAC       ESP_MAC_WIFI_STA
#endif

#include "EspnowDriver.hpp"
#include "EspmeshDriver.hpp"

/**
 * @brief The WifiManager class handles the initialization of the WiFi module.
 */
class WifiManager : public IComponent
{
private:
public:
    /**
     * @brief Initializes the WiFi module.
     *
     * @param data A pointer to the initialization data.
     */
    void Init();
};

#endif
