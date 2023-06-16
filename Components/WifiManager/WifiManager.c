#include "WifiManager.h"

#include "esp_wifi.h"
#include "esp_now.h"

void WifiManager_Init(void* pvParameters)
{
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());

    #define MAX_TX_POWER 80
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(MAX_TX_POWER));
}
/*
Mapping Table {Power, max_tx_power} = {
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
}*/