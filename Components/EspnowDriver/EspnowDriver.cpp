#include "EspnowDriver.hpp"
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

Spinlock sendProtection = Spinlock_Init;
void (*ul_callback)(const uint8_t*, const Payload*, const RSSI_Type);

void EspnowDriver_Init(void(*callback)(const uint8*, const Payload*, const RSSI_Type))
{
    LogManager_SetMinimalLevel("EspnowDriver", W);

    ul_callback = callback;
    esp_read_mac(my_esp_now_mac, ESPNOW_MAC);

    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(DataReceive) );
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_WIFI_CHANNEL;
    peer->ifidx = (wifi_interface_t)ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, broadcast_mac, ESP_NOW_ETH_ALEN);

    ESP_ERROR_CHECK( esp_now_add_peer(peer) );

    free(peer);
}

void DataSend(const uint8* dst_addr, const Payload* message)
{
    DUMMY_STATEMENT(dst_addr);
    static esp_err_t err;
    uint8 package_size = (uint8)(message->GetSize()) + ESP_NOW_ETH_ALEN;
    uint8* package = (uint8*)malloc(package_size);
    memcpy(package, broadcast_mac, ESP_NOW_ETH_ALEN);
    memcpy(package + ESP_NOW_ETH_ALEN, message->data, message->GetSize());

    // Forums said locking the send should help reduce errors
    Enter_Critical_Spinlock(sendProtection);
    err = esp_now_send(broadcast_mac, package, package_size);
    Exit_Critical_Spinlock(sendProtection);

    if(ESP_OK != err)
    {
        if(ESP_ERR_ESPNOW_NO_MEM == err)
        {
            LogManager_Log(E, "EspnowDriver", "esp_now_send: ESP_ERR_ESPNOW_NO_MEM, increase buffer");
        }
        else
        {
            ESP_ERROR_CHECK(err);
        }
    }
    free(package);
}

void DataReceive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
{
    if(ESP_NOW_ETH_ALEN >= len)
    {
        // Received a message with a broken size
        return;
    }
    if( memcmp(recv_info->des_addr, broadcast_mac, ESP_NOW_ETH_ALEN) != 0)
    {
        // Received non broadcast mac
        return;
    }

    Payload* message_data = new Payload(data, len);

    Payload* message_header = new Payload(ESP_NOW_ETH_ALEN);
    *message_data >>= *message_header;

    const uint8* dst_address = message_header->data;

    if( memcmp(dst_address, broadcast_mac,  ESP_NOW_ETH_ALEN) == 0
    ||  memcmp(dst_address, my_esp_now_mac, ESP_NOW_ETH_ALEN) == 0)
    {
        ul_callback(recv_info->src_addr, message_data, recv_info->rx_ctrl->rssi);
    }

    delete message_header;
    delete message_data;
}
