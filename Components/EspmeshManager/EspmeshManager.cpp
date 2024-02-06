#include "EspmeshManager.hpp"
#include "EspnowManager.hpp"
#include "Common.hpp"

#include "IEspmeshMessage.hpp"
#include "Messages.hpp"
#include <cstring>
#include <string>

void EspmeshManager::Init()
{
    TaskConfig config = TaskConfig(
        "EspmeshManager",
        [this]() { MainFunction(); },
        1000,
        CORE_1,
        8192,
        10
    );
    taskManager.RequestTask(config);

    internalState = MESH_INIT;
}

void EspmeshManager::MainFunction()
{
    // espnowController.ActivateNetwork();
}

void EspmeshManager::Receive(const NetIdentifier address, const std::queue<Payload> originalPayloadQueue)
{
    std::queue<Payload> payloadQueue = originalPayloadQueue;
    Payload messageTypePayload = payloadQueue.front();
    payloadQueue.pop();
    MessageType messageType = *((MessageType*)messageTypePayload.GetData());

    switch (messageType)
    {
    case MESH_ROOT_UPDATED:
    {
        ReceiveMeshRootAddress(address);
        break;
    }
    case MESH_PARENT_CONNECTED:
    case MESH_GET_NODES:
    {
        SendToRootMeshNodeConnected();
        break;
    }
    default:
        break;
    }
}

void EspmeshManager::SendToRootMeshNodeConnected()
{
    std::stack<Payload> payloadStack;

    payloadStack.push(Payload(nvsManager.GetVar<std::string>("Info", "name", "Default")));
    payloadStack.push(Payload(MESH_NODE_CONNECTED));

    NetIdentifier netId;
    rootAddress.CopyTo(netId.mac);

    lowerLayer.Send(netId, payloadStack);
}

void EspmeshManager::ReceiveMeshRootAddress(const NetIdentifier address)
{
    rootAddress = MacAddress(address.mac);
}

std::string EspmeshManager::GetMonitorData()
{
    return "EspmeshManager\n";
}
