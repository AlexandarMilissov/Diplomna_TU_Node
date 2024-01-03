#include "EspnowDriver.hpp"
#include "WifiDriver.hpp"
#include <string.h>
#include <algorithm>
#include "MacAddress.hpp"

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

    ESP_ERROR_CHECK( esp_now_init() );

    ESP_ERROR_CHECK( esp_now_register_recv_cb(EspnowDriver::InternalReceive) );
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t*)malloc(sizeof(esp_now_peer_info_t));

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_WIFI_CHANNEL;
    peer->ifidx = (wifi_interface_t)ESPNOW_WIFI_IF;
    peer->encrypt = false;
    WifiDriver::broadcastMac.CopyTo(peer->peer_addr);

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

    uint8 broadcast_mac[6];
    WifiDriver::broadcastMac.CopyTo(broadcast_mac);

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
    MacAddress received_address(recv_info->src_addr);
    if( received_address != WifiDriver::broadcastMac)
    {
        // Received non broadcast mac
        return;
    }

    Payload received_message_data = Payload(data, len);

    Payload destination_address = Payload(ESP_NOW_ETH_ALEN);
    received_message_data >>= destination_address;

    MacAddress destination_mac_address(destination_address.data);

    if(destination_mac_address == WifiDriver::broadcastMac
    || destination_mac_address == WifiDriver::myMac)
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
