#include "Common.hpp"
#include "WifiDriver.hpp"
#include "esp_wifi.h"
#include "Messages.hpp"
#include "Payload.hpp"
#include <algorithm>
#include "NetIdentifier.hpp"

MacAddress WifiDriver::myStaMac;
MacAddress WifiDriver::myApMac;
MacAddress WifiDriver::broadcastMac;

std::vector<WifiDriver*> WifiDriver::drivers;

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

    drivers.push_back(this);
}

WifiDriver::~WifiDriver()
{
    drivers.erase(std::remove(drivers.begin(), drivers.end(), this), drivers.end());
}

void WifiDriver::Init()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,  WifiDriver::ReceiveIpEvent, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, WifiDriver::ReceiveIpEvent, NULL));

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

void WifiDriver::Subscribe(IMessageReceiver& messageable)
{
    upperLayerMessageables.push_back(&messageable);
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

void WifiDriver::ReceiveIpEvent(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    for (auto driver : drivers)
    {
        driver->DistributeIpEvents(arg, event_base, event_id, event_data);
    }
}

void WifiDriver::DistributeIpEvents(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
        ReceiveIpEventStaGotIp(arg, event_base, event_id, event_data);
        break;
    case IP_EVENT_STA_LOST_IP:
        ReceiveIpEventStaLostIp(arg, event_base, event_id, event_data);
        break;
    default:
        break;
    }
}

void WifiDriver::ReceiveIpEventStaGotIp(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    NetIdentifier myNetId;
    myNetId.ip = event->ip_info.ip.addr;

    MessageType messageType = NETWORK_GOT_ADDRESS;
    std::queue<Payload> dataQueue;

    dataQueue.push(Payload((uint8*)(&messageType), sizeof(messageType)));

    for (auto messageable : upperLayerMessageables)
    {
        messageable->Receive(myNetId, dataQueue);
    }
}

void WifiDriver::ReceiveIpEventStaLostIp(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    MessageType messageType = NETWORK_LOST_ADDRESS;
    std::queue<Payload> dataQueue;
    NetIdentifier myNetId;

    dataQueue.push(Payload((uint8*)(&messageType), sizeof(messageType)));

    for (auto messageable : upperLayerMessageables)
    {
        messageable->Receive(myNetId, dataQueue);
    }
}

