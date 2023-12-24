#include "EspnowDriver.hpp"
#include "WifiManager.hpp"
#include <string.h>
#include <algorithm>

Spinlock EspnowDriver::sendLock = Spinlock_Init;
std::vector<EspnowDriver*> EspnowDriver::drivers;

EspnowDriver::EspnowDriver(LogManager& logManager) : logManager(logManager)
{
    drivers.push_back(this);
}


EspnowDriver::~EspnowDriver()
{
    drivers.erase(std::remove(drivers.begin(), drivers.end(), this), drivers.end());
}

void EspnowDriver::Init()
{
    logManager.SetMinimalLevel("EspnowDriver", W);

    esp_read_mac(my_esp_now_mac, ESPNOW_MAC);

    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(EspnowDriver::InternalReceive) );
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

void EspnowDriver::Subscribe(IMessageable& messageable)
{
    upperLayerMessageables.push_back(&messageable);
}

void EspnowDriver::Send(const Payload dst_addr, const Payload message)
{
    esp_err_t err;

    Payload package = Payload(dst_addr);
    package += message;

    // Forums said locking the send should help reduce errors
    // https://esp32.com/viewtopic.php?t=17592
    Enter_Critical_Spinlock(sendLock);

    err = esp_now_send(broadcast_mac, package.data, package.GetSize());

    Exit_Critical_Spinlock(sendLock);

    if(ESP_OK != err)
    {
        if(ESP_ERR_ESPNOW_NO_MEM == err)
        {
            logManager.Log(E, "EspnowDriver", "esp_now_send: ESP_ERR_ESPNOW_NO_MEM, increase buffer");
        }
        else
        {
            ESP_ERROR_CHECK(err);
        }
    }
}

void EspnowDriver::InternalReceive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
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

    Payload received_message_data = Payload(data, len);

    Payload destination_address = Payload(ESP_NOW_ETH_ALEN);
    received_message_data >>= destination_address;

    if( memcmp(destination_address.data, broadcast_mac,  ESP_NOW_ETH_ALEN) == 0
    ||  memcmp(destination_address.data, my_esp_now_mac, ESP_NOW_ETH_ALEN) == 0)
    {
        Payload source_address = Payload(recv_info->src_addr, ESP_NOW_ETH_ALEN);
        RSSI_Type rssi = recv_info->rx_ctrl->rssi;
        Payload rssi_payload = Payload((uint8*)(&rssi), sizeof(RSSI_Type));
        received_message_data += rssi_payload;

        for (auto driver : drivers)
        {
            driver->Receive(&source_address, &received_message_data);
        }
    }
}

void EspnowDriver::Receive(const Payload* src_addr, const Payload* message)
{
    for(auto messageable : upperLayerMessageables)
    {
        messageable->Receive(src_addr, message);
    }
}
