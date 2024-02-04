#include "EspmeshServer.hpp"
#include "IEspmeshMessage.hpp"
#include "Messages.hpp"
#include "esp_wifi.h"
#include "esp_wifi_netif.h"
#include "esp_netif.h"
#include "esp_netif_defaults.h"

void EspmeshServer::Init()
{
    logManager.Log(V, "EspmeshServer", "Init\n");
}

void EspmeshServer::Receive(const NetIdentifier address, const std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload messageTypePayload = payloadQueue.front();
    payloadQueue.pop();
    MessageType messageType = *((MessageType*)messageTypePayload.GetData());

    switch (messageType)
    {
    case MESH_ROOT_UPDATED:
    {
        ReceiveRootUpdated(payloadQueue);
        break;
    }
    case MESH_EXTERNAL_IP_ACCESS_UPDATED:
    {
        ReceiveToDsStateUpdated(payloadQueue);
        break;
    }
    case UDP_DISCOVER_REQUEST:
    {
        ReceiveUdpDiscoverRequest(address, payloadQueue);
        break;
    }
    default:
        logManager.Log(E, "EspmeshServer", "Unknown message type: %d\n", messageType);
        break;
    }

    logManager.Log(V, "EspmeshServer", "Receive\n");
}

void EspmeshServer::ReceiveUdpDiscoverRequest(NetIdentifier netId, std::queue<Payload> payloadQueue)
{
    NetSocket socket = netId.socket;
    uint8 address[4] = { 0 };
    memcpy(address, &socket.ip, 4);
    uint16 port = socket.port;
    logManager.Log(I, "EspmeshServer", "Received ReceiveUdpDiscoverRequest from: %d.%d.%d.%d:%d",
        address[0], address[1], address[2], address[3],
        port);

    SendUdpDiscoverResponse(netId);
}

void EspmeshServer::SendUdpDiscoverResponse(NetIdentifier netId)
{
    // Prepare the response data
    #define ID_SIZE 6
    MessageType messageType = UDP_DISCOVER_RESPONSE;
    uint8 id[ID_SIZE] = {
        nvsManager.GetVar<uint8>("espMesh", "id0", 0x77),
        nvsManager.GetVar<uint8>("espMesh", "id1", 0x77),
        nvsManager.GetVar<uint8>("espMesh", "id2", 0x77),
        nvsManager.GetVar<uint8>("espMesh", "id3", 0x77),
        nvsManager.GetVar<uint8>("espMesh", "id4", 0x77),
        nvsManager.GetVar<uint8>("espMesh", "id5", 0x77)
    };

    std::string name = nvsManager.GetVar<std::string>("Info", "name", "Default");

    // Compose the response
    std::stack<Payload> payloadStack;
    payloadStack.push(Payload(name));
    payloadStack.push(Payload((void*)id, ID_SIZE));
    payloadStack.push(Payload((void*)(&messageType), sizeof(MessageType)));

    outerNetwork.Send(netId, payloadStack);
}

void EspmeshServer::ReceiveToDsStateUpdated(std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload toDsStatePayload = payloadQueue.front();
    payloadQueue.pop();

    bool isExternalNetworkReachable = *((bool*)toDsStatePayload.GetData());

    if(isExternalNetworkReachable)
    {
        logManager.Log(I, "EspmeshServer", "External network is reachable\n");
    }
    else
    {
        logManager.Log(I, "EspmeshServer", "External network is not reachable\n");
    }
}

void EspmeshServer::ReceiveRootUpdated(std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload rootAddressPayload = payloadQueue.front();
    payloadQueue.pop();

    bool isRoot = *((bool*)rootAddressPayload.GetData());

    if(isRoot && isServer)
    {
        logManager.Log(E, "EspmeshServer", "Root updated to this node, but this node is already a server\n");

        // Do nothing, the current node is already a server
    }
    else if(isRoot && !isServer)
    {
        isServer = true;
        logManager.Log(I, "EspmeshServer", "Root updated to this node, this node is now a server\n");

        // Start the server
        StartServer();
    }
    else if(!isRoot && isServer)
    {
        isServer = false;
        logManager.Log(I, "EspmeshServer", "Root updated to another node, this node is no longer a server\n");

        // Stop the server
        StopServer();
    }
    else if(!isRoot && !isServer)
    {
        logManager.Log(I, "EspmeshServer", "Root updated to another node, this node is not a server\n");

        // Do nothing, the current node is not a server and will not become one
    }
}


void EspmeshServer::StartServer()
{

}

void EspmeshServer::StopServer()
{
    logManager.Log(V, "EspmeshServer", "StopServer\n");
}


std::string EspmeshServer::GetMonitorData()
{
    return "EspmeshServer";
}
