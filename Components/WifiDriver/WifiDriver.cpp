#include "Common.hpp"
#include "WifiDriver.hpp"
#include "esp_wifi.h"

MacAddress WifiDriver::myStaMac;
MacAddress WifiDriver::myApMac;
MacAddress WifiDriver::broadcastMac;

WifiDriver::WifiDriver()
{
    uint8 my_esp_now_mac[6];
    esp_read_mac(my_esp_now_mac, ESP_MAC_WIFI_STA);
    myStaMac = MacAddress(my_esp_now_mac);

    uint8 my_ap_mac[6];
    esp_read_mac(my_ap_mac, ESP_MAC_WIFI_SOFTAP);
    myApMac = MacAddress(my_ap_mac);

    uint8 broadcast_esp_now_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    broadcastMac = MacAddress(broadcast_esp_now_mac);
}

#include "esp_log.h"

void WifiDriver::Init()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Added to fix the netif problem of wifi mesh.
    (void)esp_netif_create_default_wifi_sta();

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

MacAddress WifiDriver::GetBroadcastMac()
{
    return broadcastMac;
}

MacAddress WifiDriver::GetMyStaMac()
{
    return myStaMac;
}

MacAddress WifiDriver::GetMyApMac()
{
    return myApMac;
}
