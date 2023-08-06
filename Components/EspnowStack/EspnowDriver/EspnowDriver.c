#include "EspnowDriver.h"
#include <string.h>
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

void (*ul_callback)(uint8_t*, Message*, RSSI_Type);

void EspnowDriver_Init(void(*callback)(uint8_t*, Message*, RSSI_Type))
{
    esp_log_level_set("EspnowDriver", ESP_LOG_WARN);

    ul_callback = callback;
    esp_read_mac(my_esp_now_mac, ESPNOW_MAC);

    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(DataReceive) );
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );
    
    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_WIFI_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, broadcast_mac, ESP_NOW_ETH_ALEN);

    ESP_ERROR_CHECK( esp_now_add_peer(peer) );

    free(peer);
}

void DataSend(uint8* dst_addr, Message* message)
{
    uint8 package_size = message->data_size + ESP_NOW_ETH_ALEN;
    uint8* package = (uint8*)malloc(package_size);
    memcpy(package, broadcast_mac, ESP_NOW_ETH_ALEN);
    memcpy(package + ESP_NOW_ETH_ALEN, message->data, message->data_size);
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_send(broadcast_mac, package, package_size));
    free(package);
}

void DataReceive(const esp_now_recv_info_t *recv_info, const uint8_t *package, int len)
{
    if( memcmp(recv_info->des_addr, broadcast_mac, ESP_NOW_ETH_ALEN) != 0)
    {
        ESP_LOGW("EspnowDriver", "Received non broadcast mac");
        return;
    }

    Message* message_original = MessageInit(len);
    memcpy(message_original->data, package, len);

    Message* message_header = MessageInit(ESP_NOW_ETH_ALEN);
    Message* message_data = MessageInit(0);
    MessageDecompose(message_original, message_header, message_data);

    uint8* dst_adress = message_header->data;

    if( memcmp(dst_adress, broadcast_mac,  ESP_NOW_ETH_ALEN) == 0 
    ||  memcmp(dst_adress, my_esp_now_mac, ESP_NOW_ETH_ALEN) == 0)
    {
        uint8 data_size = len - ESP_NOW_ETH_ALEN;
        uint8* data = (uint8*)malloc(data_size);
        memcpy(data, package + ESP_NOW_ETH_ALEN, data_size);

        ul_callback(recv_info->src_addr, message_data, recv_info->rx_ctrl->rssi);
    }

    MessageDeinit(message_original);
    MessageDeinit(message_header);
    MessageDeinit(message_data);
}
