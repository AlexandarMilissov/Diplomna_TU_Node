#include "EspmeshDriver.hpp"
#include "Common.hpp"
#include "IEspmeshMessage.hpp"
#include "Messages.hpp"

#include "esp_mesh.h"
#include "esp_event.h"

#include <string>
#include <cstring>
#include <algorithm>

std::vector<EspmeshDriver*> EspmeshDriver::drivers;

EspmeshDriver::EspmeshDriver(
    LogManager& logManager,
    IScheduler& scheduler,
    NvsManager& nvsManager
) :
    logManager(logManager),
    scheduler(scheduler),
    nvsManager(nvsManager)
{
    drivers.push_back(this);
}

EspmeshDriver::~EspmeshDriver()
{
    // Remove this driver from drivers
    drivers.erase(std::remove(drivers.begin(), drivers.end(), this), drivers.end());
    free(receiveBuffer);
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
        .channel = nvsManager.GetVar<uint8>("espMesh", "channel", CONFIG_MESH_CHANNEL),
        .allow_channel_switch = false,
        .mesh_id = {
            .addr = {
                nvsManager.GetVar<uint8>("espMesh", "id0", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id1", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id2", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id3", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id4", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id5", 0x77)
            },
        },
        .router = {
            .ssid_len = 0,
            .password = { 0 }
        },
        .mesh_ap = {
            .password = { 0 },
            .max_connection = nvsManager.GetVar<uint8>("espMesh", "maxConn", CONFIG_MESH_AP_CONNECTIONS),
            .nonmesh_max_connection = nvsManager.GetVar<uint8>("espMesh", "nonMeshMaxConn", CONFIG_MESH_NON_MESH_AP_CONNECTIONS)
        },
        .crypto_funcs = &g_wifi_default_mesh_crypto_funcs
    };

    std::string meshApPassword = nvsManager.GetVar<std::string>("espMesh", "meshApPass", CONFIG_MESH_AP_PASSWD);
    memcpy((uint8_t *) &cfg.mesh_ap.password, meshApPassword.c_str(), meshApPassword.length());
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode((wifi_auth_mode_t)nvsManager.GetVar<uint8>("espMesh", "authMode", CONFIG_MESH_AP_AUTHMODE)));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
}

void EspmeshDriver::ConnectRouter()
{
    // Enable mesh power saving function
    ESP_ERROR_CHECK(esp_mesh_disable_ps());
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(30));
    ESP_ERROR_CHECK(esp_mesh_set_type(MESH_IDLE));
    ESP_ERROR_CHECK(esp_mesh_allow_root_conflicts(false));
    ESP_ERROR_CHECK(esp_mesh_fix_root(false));

    std::string           ssid = nvsManager.GetVar<std::string>("espMesh", "ssid",       CONFIG_MESH_ROUTER_SSID);
    std::string       password = nvsManager.GetVar<std::string>("espMesh", "routerPass", CONFIG_MESH_ROUTER_PASSWD);
    std::string meshApPassword = nvsManager.GetVar<std::string>("espMesh", "meshApPass", CONFIG_MESH_AP_PASSWD);

    mesh_cfg_t cfg = {
        .channel = nvsManager.GetVar<uint8>("espMesh", "channel", CONFIG_MESH_CHANNEL),
        .allow_channel_switch = false,
        .mesh_id = {
            .addr = {
                nvsManager.GetVar<uint8>("espMesh", "id0", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id1", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id2", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id3", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id4", 0x77),
                nvsManager.GetVar<uint8>("espMesh", "id5", 0x77)
            },
        },
        .router = { },
        .mesh_ap = {
            .password = { 0 },
            .max_connection = nvsManager.GetVar<uint8>("espMesh", "maxConn", CONFIG_MESH_AP_CONNECTIONS),
            .nonmesh_max_connection = nvsManager.GetVar<uint8>("espMesh", "nonMeshMaxConn", CONFIG_MESH_NON_MESH_AP_CONNECTIONS)
        },
        .crypto_funcs = &g_wifi_default_mesh_crypto_funcs
    };

    cfg.router.ssid_len = ssid.length();
    memcpy((uint8_t *) &cfg.router.ssid, ssid.c_str(), cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, password.c_str(), password.length());
    memcpy((uint8_t *) &cfg.mesh_ap.password, meshApPassword.c_str(), meshApPassword.length());

    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode((wifi_auth_mode_t)nvsManager.GetVar<uint8>("espMesh", "authMode", CONFIG_MESH_AP_AUTHMODE)));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
}

void EspmeshDriver::Init()
{
    logManager.SetMinimalLevel("EspmeshDriver", I);

    receiveBufferLength = nvsManager.GetVar<uint16>("espMesh", "recvBuffLen", 1500);
    receiveBuffer = (uint8*)malloc(receiveBufferLength);

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

    TaskConfig taskConfigReceive(
        "EspmeshDriver Receive",
        [this](){ Receive(); },
        10,
        0,
        4096,
        10
    );

    scheduler.RequestTask(taskConfigReceive);
}

void EspmeshDriver::Subscribe(IMessageReceiver& component)
{
    upperLayerMessageables.push_back(&component);
}

void EspmeshDriver::Send(const NetIdentifier netAddress, const std::stack<Payload> dataStack)
{
    MacAddress address = MacAddress(netAddress.mac);

    if(address == myAddress || address == broadcastAddress)
    {
        std::queue<Payload> dataQueue;
        std::stack<Payload> dataStackCopy = dataStack;

        while (!dataStackCopy.empty()) {
            dataQueue.push(dataStackCopy.top());
            dataStackCopy.pop();
        }

        NetIdentifier netId;
        address.CopyTo(netId.mac);

        for(auto messageable : upperLayerMessageables)
        {
            messageable->Receive(netId, dataQueue);
        }
    }

    esp_err_t err;
    mesh_addr_t meshAddress;
    mesh_data_t meshData;

    address.CopyTo(meshAddress.addr);
    Payload data = Payload::Compose(dataStack);
    meshData.data = (uint8*)data.GetData();
    meshData.size = data.GetSize();
    meshData.proto = MESH_PROTO_BIN;
    meshData.tos = MESH_TOS_P2P;
    int flags = MESH_DATA_P2P;
    err = esp_mesh_send(&meshAddress, &meshData, flags, NULL, 0);
    if(ESP_OK != err)
    {
        logManager.Log(E, "EspmeshDriver", "Send data fail: %s", esp_err_to_name(err));
    }
}

void EspmeshDriver::SendBroadcast(const std::stack<Payload> data)
{
    NetIdentifier netId;
    broadcastAddress.CopyTo(netId.mac);
    Send(netId, data);
}

void EspmeshDriver::Receive()
{
    mesh_addr_t from;
    mesh_data_t meshData;
    meshData.data = receiveBuffer;
    meshData.size = receiveBufferLength;

    int flag = 0;
    uint8 operationsOfCycle = 0xFF;

    while(operationsOfCycle > 0)
    {
        esp_err_t err = esp_mesh_recv(&from, &meshData, 0, &flag, NULL, 0);

        if(ESP_ERR_MESH_TIMEOUT == err)
        {
            // No message received
            break;
        }
        else if(ESP_OK != err)
        {
            logManager.Log(E, "EspmeshDriver", "Receive data fail: %s", esp_err_to_name(err));
            return;
        }

        MacAddress address = MacAddress(from.addr);
        NetIdentifier netId;
        address.CopyTo(netId.mac);
        std::queue<Payload> dataQueue = Payload::Decompose(meshData.data, meshData.size);

        for(auto messageable : upperLayerMessageables)
        {
            messageable->Receive(netId, dataQueue);
        }

        operationsOfCycle--;
    }
}

void EspmeshDriver::DistributeMeshEvents(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    LogSeverity logSeverity = I;

    switch((mesh_event_id_t)event_id)
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
            ReceiveMeshEventChildDisconnected(arg, event_base, event_id, event_data);
            break;
        case MESH_EVENT_ROUTING_TABLE_ADD:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROUTING_TABLE_ADD");
            break;
        case MESH_EVENT_ROUTING_TABLE_REMOVE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_ROUTING_TABLE_REMOVE");
            break;
        case MESH_EVENT_PARENT_CONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PARENT_CONNECTED");
            ReceiveMeshEventParentConnected(arg, event_base, event_id, event_data);
            break;
        case MESH_EVENT_PARENT_DISCONNECTED:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_PARENT_DISCONNECTED");
            ReceiveMeshEventParentDisconnected(arg, event_base, event_id, event_data);
            break;
        case MESH_EVENT_NO_PARENT_FOUND:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_NO_PARENT_FOUND");
            break;
        case MESH_EVENT_LAYER_CHANGE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_LAYER_CHANGE");
            break;
        case MESH_EVENT_TODS_STATE:
            logManager.Log(logSeverity, "EspmeshDriver", "Mesh event: MESH_EVENT_TODS_STATE");
            ReceiveMeshEventToDsState(arg, event_base, event_id, event_data);
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
    isRoot = esp_mesh_is_root();

    if(isRoot)
    {
        rootAddress = myAddress;
    }
    else
    {
        rootAddress = MacAddress(((mesh_event_root_address_t*)event_data)->addr);
    }

    std::queue<Payload> dataQueue;
    MessageType messageType = MESH_ROOT_UPDATED;
    NetIdentifier netId;
    rootAddress.CopyTo(netId.mac);

    dataQueue.push(Payload((uint8*)(&messageType),  sizeof(messageType)));
    dataQueue.push(Payload((uint8*)(&isRoot),       sizeof(isRoot)));

    for(auto messageable : upperLayerMessageables)
    {
        messageable->Receive(netId, dataQueue);
    }
}

void EspmeshDriver::ReceiveMeshEventChildDisconnected(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    mesh_event_child_disconnected_t* child_disconnected = (mesh_event_child_disconnected_t*)event_data;
    logManager.Log(W, "EspmeshDriver", "Child disconnected. Reason is %d", child_disconnected->reason);
}

void EspmeshDriver::ReceiveMeshEventParentConnected(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    mesh_addr_t meshAddress;
    esp_mesh_get_parent_bssid(&meshAddress);
    parentAddress = MacAddress(meshAddress.addr);
}

void EspmeshDriver::ReceiveMeshEventParentDisconnected(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    parentAddress = MacAddress();
}

void EspmeshDriver::ReceiveMeshEventToDsState(void *arg, esp_event_base_t event_base, sint32 event_id, void *event_data)
{
    mesh_event_toDS_state_t* toDsState = (mesh_event_toDS_state_t*)event_data;

    bool toDsStateBool = toDsState;
    NetIdentifier myNetId;
    myAddress.CopyTo(myNetId.mac);

    std::queue<Payload> dataQueue;
    MessageType messageType = MESH_EXTERNAL_IP_ACCESS_UPDATED;

    dataQueue.push(Payload((uint8*)(&messageType)  , sizeof(messageType)));
    dataQueue.push(Payload((uint8*)(&toDsStateBool), sizeof(toDsStateBool)));

    for(auto messageable : upperLayerMessageables)
    {
        messageable->Receive(myNetId, dataQueue);
    }
}
