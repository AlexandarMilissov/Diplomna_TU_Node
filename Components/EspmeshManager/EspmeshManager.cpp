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

    // TaskConfig configSendKeepAlive = TaskConfig(
    //     "EspmeshManagerSendKeepAlive",
    //     [this]() { MainFunctionSendKeepAlive(); },
    //     3000,
    //     CORE_1,
    //     8192,
    //     10
    // );

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
        rootAddress = MacAddress(address.mac);
        break;
    }
    default:
        break;
    }
}

std::string EspmeshManager::GetMonitorData()
{
    return "EspmeshManager\n";
}
