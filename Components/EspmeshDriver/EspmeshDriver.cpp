#include "EspmeshDriver.hpp"
#include "Common.hpp"

#include "esp_mesh.h"

#include "esp_event.h"

#include <string>

#define RX_SIZE          (1500)
#define TX_SIZE          (1460)
static uint8_t tx_buf[TX_SIZE] = { 0, };
static uint8_t rx_buf[RX_SIZE] = { 0, };

std::vector<EspmeshDriver*> EspmeshDriver::drivers;

EspmeshDriver::EspmeshDriver(
    NvsManager& nvsManager,
    LogManager& logManager,
    IScheduler& scheduler
) :
    nvsManager(nvsManager),
    logManager(logManager),
    scheduler(scheduler)
{
    drivers.push_back(this);
}

void EspmeshDriver::ReceiveMeshEvent(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    for(EspmeshDriver* driver : drivers)
    {
        driver->DistributeMeshEvents(arg, event_base, event_id, event_data);
    }
}

void EspmeshDriver::ConnectRouterless()
{
    // Disable mesh power saving function
    ESP_ERROR_CHECK(esp_mesh_disable_ps());
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(30));
    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_ROOT));
    ESP_ERROR_CHECK(esp_mesh_allow_root_conflicts(true));
    ESP_ERROR_CHECK(esp_mesh_fix_root(true));

    mesh_cfg_t cfg = {
        .channel = nvsManager.GetVar<uint8>("espMshChn", CONFIG_MESH_CHANNEL),
        .allow_channel_switch = false,
        .mesh_id = {
            .addr = {
                nvsManager.GetVar<uint8>("espMshId0", 0x77),
                nvsManager.GetVar<uint8>("espMshId1", 0x77),
                nvsManager.GetVar<uint8>("espMshId2", 0x77),
                nvsManager.GetVar<uint8>("espMshId3", 0x77),
                nvsManager.GetVar<uint8>("espMshId4", 0x77),
                nvsManager.GetVar<uint8>("espMshId5", 0x77)
            },
        },
        .router = {
            .ssid_len = 0,
            .password = { 0 }
        },
        .mesh_ap = {
            .password = { 0 },
            .max_connection = nvsManager.GetVar<uint8>("espMshMxCon", CONFIG_MESH_AP_CONNECTIONS),
            .nonmesh_max_connection = nvsManager.GetVar<uint8>("espMshNMshCon", CONFIG_MESH_NON_MESH_AP_CONNECTIONS)
        },
        .crypto_funcs = &g_wifi_default_mesh_crypto_funcs
    };

    std::string meshApPassword = nvsManager.GetVar<std::string>("espMshApPsd", CONFIG_MESH_AP_PASSWD);
    memcpy((uint8_t *) &cfg.mesh_ap.password, meshApPassword.c_str(), meshApPassword.length());
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode((wifi_auth_mode_t)nvsManager.GetVar<uint8>("espMshAuthMode", CONFIG_MESH_AP_AUTHMODE)));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
}

void EspmeshDriver::ConnectRouter()
{
    // Enable mesh power saving function
    ESP_ERROR_CHECK(esp_mesh_enable_ps());
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(30));
    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_IDLE));
    ESP_ERROR_CHECK(esp_mesh_allow_root_conflicts(false));
    ESP_ERROR_CHECK(esp_mesh_fix_root(false));

    std::string ssid = nvsManager.GetVar<std::string>("espMshSsid", CONFIG_MESH_ROUTER_SSID);
    std::string password = nvsManager.GetVar<std::string>("espMshPsd", CONFIG_MESH_ROUTER_PASSWD);
    std::string meshApPassword = nvsManager.GetVar<std::string>("espMshApPsd", CONFIG_MESH_AP_PASSWD);

    mesh_cfg_t cfg = {
        .channel = nvsManager.GetVar<uint8>("espMshChn", CONFIG_MESH_CHANNEL),
        .allow_channel_switch = false,
        .mesh_id = {
            .addr = {
                nvsManager.GetVar<uint8>("espMshId0", 0x77),
                nvsManager.GetVar<uint8>("espMshId1", 0x77),
                nvsManager.GetVar<uint8>("espMshId2", 0x77),
                nvsManager.GetVar<uint8>("espMshId3", 0x77),
                nvsManager.GetVar<uint8>("espMshId4", 0x77),
                nvsManager.GetVar<uint8>("espMshId5", 0x77)
            },
        },
        .router = { },
        .mesh_ap = {
            .password = { 0 },
            .max_connection = nvsManager.GetVar<uint8>("espMshMxCon", CONFIG_MESH_AP_CONNECTIONS),
            .nonmesh_max_connection = nvsManager.GetVar<uint8>("espMshNMshCon", CONFIG_MESH_NON_MESH_AP_CONNECTIONS)
        },
        .crypto_funcs = &g_wifi_default_mesh_crypto_funcs
    };

    cfg.router.ssid_len = ssid.length();
    memcpy((uint8_t *) &cfg.router.ssid, ssid.c_str(), cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, password.c_str(), password.length());
    memcpy((uint8_t *) &cfg.mesh_ap.password, meshApPassword.c_str(), meshApPassword.length());

    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode((wifi_auth_mode_t)nvsManager.GetVar<uint8>("espMshAuthMode", CONFIG_MESH_AP_AUTHMODE)));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
}

void EspmeshDriver::Init()
{
    logManager.SetMinimalLevel("EspmeshDriver", I);
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    /*  register mesh events handler */
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, EspmeshDriver::ReceiveMeshEvent, NULL));

    ConnectRouter();

    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());

    logManager.Log(I, "EspmeshDriver", "mesh starts successfully, heap:%" PRId32 ", %s<%d>%s, ps:%d",  esp_get_minimum_free_heap_size(),
            esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed",
            esp_mesh_get_topology(), esp_mesh_get_topology() ? "(chain)":"(tree)", esp_mesh_is_ps_enabled());

    TaskConfig taskConfig(
        "EspmeshDriver Receive",
        [this](){ Receive(NULL, NULL); },
        10,
        0,
        4096,
        10
    );

    scheduler.RequestTask(taskConfig);
}

void EspmeshDriver::Subscribe(IMessageable& component)
{

}

void EspmeshDriver::Send(const Payload address, const Payload data)
{
    esp_err_t err;
    mesh_addr_t to;
    mesh_data_t mesh_data;
    memcpy(to.addr, address.data, 6);
    mesh_data.data = data.data;
    mesh_data.size = data.GetSize();
    mesh_data.proto = MESH_PROTO_BIN;
    mesh_data.tos = MESH_TOS_P2P;
    err = esp_mesh_send(&to, &mesh_data, MESH_DATA_P2P, NULL, 0);
}

void EspmeshDriver::Receive(const Payload*, const Payload*)
{
    mesh_rx_pending_t pending;
    mesh_addr_t from;
    esp_mesh_get_rx_pending(&pending);
    int flags = MESH_DATA_FROMDS;
    for(int i = 0; i < pending.toDS; i++)
    {
        mesh_data_t data;
        esp_err_t err = esp_mesh_recv(&from, &data, 0, &flags, NULL, 0);
        if(err == ESP_OK)
        {
            logManager.Log(I, "EspmeshDriver", "Receive data successfully.");
        }
        else
        {
            logManager.Log(E, "EspmeshDriver", "Receive data fail.");
        }
    }
}


#include "esp_log.h" // Include the missing header file

void EspmeshDriver::DistributeMeshEvents(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    LogSeverity logSeverity = I;
    mesh_event_id_t event_type = (mesh_event_id_t)event_id;

    switch(event_type)
    {
        case MESH_EVENT_STARTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_STARTED");
            break;
        case MESH_EVENT_STOPPED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_STOPPED");
            break;
        case MESH_EVENT_CHANNEL_SWITCH:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_MESH_EVENT_CHANNEL_SWITCH");
            break;
        case MESH_EVENT_CHILD_CONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_CHILD_CONNECTED");
            break;
        case MESH_EVENT_CHILD_DISCONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_CHILD_DISCONNECTED");
            break;
        case MESH_EVENT_ROUTING_TABLE_ADD:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROUTING_TABLE_ADD");
            break;
        case MESH_EVENT_ROUTING_TABLE_REMOVE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROUTING_TABLE_REMOVE");
            break;
        case MESH_EVENT_PARENT_CONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PARENT_CONNECTED");
            break;
        case MESH_EVENT_PARENT_DISCONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PARENT_DISCONNECTED");
            break;
        case MESH_EVENT_NO_PARENT_FOUND:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_NO_PARENT_FOUND");
            break;
        case MESH_EVENT_LAYER_CHANGE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_LAYER_CHANGE");
            break;
        case MESH_EVENT_TODS_STATE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_TODS_STATE");
            break;
        case MESH_EVENT_ROOT_ADDRESS:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROOT_ADDRESS");
            ReceiveMeshEventRootAddress(arg, event_base, event_id, event_data);
            break;
        case MESH_EVENT_VOTE_STARTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_VOTE_STARTED");
            break;
        case MESH_EVENT_VOTE_STOPPED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_VOTE_STOPPED");
            break;
        case MESH_EVENT_ROOT_SWITCH_REQ:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROOT_SWITCH_REQ");
            break;
        case MESH_EVENT_ROOT_SWITCH_ACK:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROOT_SWITCH_ACK");
            break;
        case MESH_EVENT_ROOT_ASKED_YIELD:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROOT_ASKED_YIELD");
            break;
        case MESH_EVENT_ROOT_FIXED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROOT_FIXED");
            break;
        case MESH_EVENT_SCAN_DONE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_SCAN_DONE");
            break;
        case MESH_EVENT_NETWORK_STATE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_NETWORK_STATE");
            break;
        case MESH_EVENT_STOP_RECONNECTION:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_STOP_RECONNECTION");
            break;
        case MESH_EVENT_FIND_NETWORK:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_FIND_NETWORK");
            break;
        case MESH_EVENT_ROUTER_SWITCH:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROUTER_SWITCH");
            break;
        case MESH_EVENT_PS_PARENT_DUTY:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PS_PARENT_DUTY");
            break;
        case MESH_EVENT_PS_CHILD_DUTY:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PS_CHILD_DUTY");
            break;
        case MESH_EVENT_PS_DEVICE_DUTY:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PS_PARENT_SWITCH");
            break;
        default:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: Unknown event");
            break;
    }
}

void EspmeshDriver::ReceiveMeshEventRootAddress(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    std::string textMessage;
    if(esp_mesh_is_root())
    {
        textMessage = "Root address acquired. We are root.";
    }
    else
    {
        mesh_event_root_address_t* root_address = (mesh_event_root_address_t*)event_data;
        textMessage = "Root address acquired. Root is ";
        textMessage +=
            std::to_string(root_address->addr[0]) + ":" +
            std::to_string(root_address->addr[1]) + ":" +
            std::to_string(root_address->addr[2]) + ":" +
            std::to_string(root_address->addr[3]) + ":" +
            std::to_string(root_address->addr[4]) + ":" +
            std::to_string(root_address->addr[5]);
    }
    logManager.Log(W, "EspmeshDriver", textMessage.c_str());
}
