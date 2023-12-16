#include "EspnowDriver.hpp"
#include <string.h>
#include "WifiManager.hpp"

Spinlock EspnowDriver::sendLock = Spinlock_Init;
std::vector<EspnowCallback> EspnowDriver::ul_callbacks = std::vector<EspnowCallback>();

void EspnowDriver::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

    LogManager::SetMinimalLevel("EspnowDriver", W);

    esp_read_mac(my_esp_now_mac, ESPNOW_MAC);

    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(EspnowDriver::Receive) );
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

void EspnowDriver::Subscribe(EspnowCallback callback)
{
    ul_callbacks.push_back(callback);
}

void EspnowDriver::Send(const uint8* dst_addr, const Payload* message)
{
    DUMMY_STATEMENT(dst_addr);
    static esp_err_t err;
    uint8 package_size = (uint8)(message->GetSize()) + ESP_NOW_ETH_ALEN;
    uint8* package = (uint8*)malloc(package_size);
    memcpy(package, broadcast_mac, ESP_NOW_ETH_ALEN);
    memcpy(package + ESP_NOW_ETH_ALEN, message->data, message->GetSize());

    // Forums said locking the send should help reduce errors
    Enter_Critical_Spinlock(sendLock);
    err = esp_now_send(broadcast_mac, package, package_size);
    Exit_Critical_Spinlock(sendLock);

    if(ESP_OK != err)
    {
        if(ESP_ERR_ESPNOW_NO_MEM == err)
        {
            LogManager::Log(E, "EspnowDriver", "esp_now_send: ESP_ERR_ESPNOW_NO_MEM, increase buffer");
        }
        else
        {
            ESP_ERROR_CHECK(err);
        }
    }
    free(package);
}

void EspnowDriver::Receive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
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
        for(auto ul_callback : ul_callbacks)
        {
            ul_callback(recv_info->src_addr, message_data, recv_info->rx_ctrl->rssi);
        }
    }

    delete message_header;
    delete message_data;
}
