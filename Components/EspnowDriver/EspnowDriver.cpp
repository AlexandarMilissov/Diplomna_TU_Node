#include "EspnowDriver.hpp"
#include "WifiDriver.hpp"
#include <string.h>
#include <algorithm>
/* ESPNOW can work in both station and softap mode. */
#if CONFIG_WIFI_AP_ENABLED
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#define GET_MAC_FUNCTION GetMyApMac
#else
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#define GET_MAC_FUNCTION GetMyStaMac
#endif

Spinlock EspnowDriver::sendLock = Spinlock_Init;
std::vector<EspnowDriver*> EspnowDriver::drivers;
MacAddress EspnowDriver::myEspnowMac;
MacAddress EspnowDriver::broadcastEspnowMac;
MacAddress EspnowDriver::communicationChannelEspnowMac;

EspnowDriver::EspnowDriver(LogManager& logManager) : logManager(logManager)
{
    drivers.push_back(this);
    myEspnowMac = WifiDriver::GET_MAC_FUNCTION();
    broadcastEspnowMac = WifiDriver::GetBroadcastMac();
    communicationChannelEspnowMac = WifiDriver::GetBroadcastMac();
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
    communicationChannelEspnowMac.CopyTo(peer->peer_addr);

    ESP_ERROR_CHECK( esp_now_add_peer(peer) );

    free(peer);
}

void EspnowDriver::Subscribe(IMessageable& messageable)
{
    upperLayerMessageables.push_back(&messageable);
}

void EspnowDriver::Send(const MacAddress dst_addr, const Payload message)
{
    esp_err_t err;

    Payload package = Payload(dst_addr);
    package += message;

    // Forums said locking the send should help reduce errors
    // https://esp32.com/viewtopic.php?t=17592

    uint8 communication_channel_mac[6];
    communicationChannelEspnowMac.CopyTo(communication_channel_mac);

    Enter_Critical_Spinlock(sendLock);

    err = esp_now_send(communication_channel_mac, package.data, package.GetSize());

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

void EspnowDriver::SendBroadcast(const Payload message)
{
    Send(broadcastEspnowMac, message);
}

void EspnowDriver::InternalReceive(const esp_now_recv_info_t *recv_info, const uint8 *data, int len)
{
    if(ESP_NOW_ETH_ALEN >= len)
    {
        // Received a message with a broken size
        return;
    }
    MacAddress received_address(recv_info->src_addr);
    if(received_address != communicationChannelEspnowMac)
    {
        // Received non broadcast mac
        return;
    }

    Payload received_message_data = Payload(data, len);

    Payload destination_address = Payload(ESP_NOW_ETH_ALEN);
    received_message_data >>= destination_address;

    MacAddress destination_mac_address(destination_address);

    if(destination_mac_address == communicationChannelEspnowMac
    || destination_mac_address == myEspnowMac)
    {
        MacAddress source_address = MacAddress(recv_info->src_addr);
        RSSI_Type rssi = recv_info->rx_ctrl->rssi;
        Payload rssi_payload = Payload((uint8*)(&rssi), sizeof(RSSI_Type));
        received_message_data += rssi_payload;

        for (auto driver : drivers)
        {
            driver->Receive(source_address, received_message_data);
        }
    }
}

void EspnowDriver::Receive(const MacAddress address, const Payload message)
{
    for(auto messageable : upperLayerMessageables)
    {
        messageable->Receive(address, message);
    }
}
