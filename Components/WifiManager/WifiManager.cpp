#include "WifiManager.hpp"
#include "Common.hpp"
#include "esp_wifi.h"

bool WifiManager::isInit = false;

void WifiManager::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

#if 0
    ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
#endif
    ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N) );

    #define MAX_TX_POWER 80
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(MAX_TX_POWER));

    isInit = true;
}

bool WifiManager::IsInit()
{
    return isInit;
}


#if 0
Mapping Table
{Power, max_tx_power} = {
    {8, 2},
    {20, 5},
    {28, 7},
    {34, 8},
    {44, 11},
    {52, 13},
    {56, 14},
    {60, 15},
    {66, 16},
    {72, 18},
    {80, 20}
}
#endif