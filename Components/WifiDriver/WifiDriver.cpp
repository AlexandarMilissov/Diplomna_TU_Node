#include "Common.hpp"
#include "WifiDriver.hpp"
#include "esp_wifi.h"

MacAddress WifiDriver::myMac;
MacAddress WifiDriver::broadcastMac;

WifiDriver::WifiDriver()
{
    uint8 my_esp_now_mac[6];
    esp_read_mac(my_esp_now_mac, ESPNOW_MAC);
    myMac = MacAddress(my_esp_now_mac);

    uint8 broadcast_esp_now_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    broadcastMac = MacAddress(broadcast_esp_now_mac);
}

void WifiDriver::Init()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

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
}
