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

void EspmeshServer::Receive(const MacAddress address, const std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload messageTypePayload = payloadQueue.front();
    payloadQueue.pop();
    MessageType messageType = *((MessageType*)messageTypePayload.GetData());

    switch (messageType)
    {
    case MESH_ROOT_UPDATED:
    {
        RootUpdated(payloadQueue);
        break;
    }
    case MESH_EXTERNAL_IP_ACCESS_UPDATED:
    {
        ToDsStateUpdated(payloadQueue);
        break;
    }
    default:
        break;
    }

    logManager.Log(V, "EspmeshServer", "Receive\n");
}

void EspmeshServer::ToDsStateUpdated(std::queue<Payload> originalPayloadQueue)
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

void EspmeshServer::RootUpdated(std::queue<Payload> originalPayloadQueue)
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
