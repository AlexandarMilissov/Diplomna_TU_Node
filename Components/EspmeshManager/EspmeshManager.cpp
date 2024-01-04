#include "EspmeshManager.hpp"
#include "EspnowManager.hpp"
#include "Common.hpp"

#include "IEspmeshMessage.hpp"
#include <cstring>
#include <string>

void EspmeshManager::Init()
{
    driver.Subscribe(*this);

    TaskConfig config = TaskConfig(
        "EspmeshManager",
        [this]() { MainFunction(); },
        1000,
        CORE_1,
        8192,
        10
    );
    taskManager.RequestTask(config);

    TaskConfig configSendKeepAlive = TaskConfig(
        "EspmeshManagerSendKeepAlive",
        [this]() { MainFunctionSendKeepAlive(); },
        3000,
        CORE_1,
        8192,
        10
    );

    internalState = MESH_INIT;
}

void EspmeshManager::MainFunction()
{
    // espnowController.ActivateNetwork();
}

void EspmeshManager::MainFunctionSendKeepAlive()
{
    switch (internalState)
    {
    case MESH_NON_ROOT:
    {
        logManager.Log(W, "EspmeshManager", "Sending MESH_KEEP_ALIVE\n");
        EspMeshApplication target = MESH;
        EspMeshMessageType messageType = MESH_KEEP_ALIVE;
        Payload data = Payload((uint8*)(&target), sizeof(target));
        data += Payload((uint8*)(&messageType), sizeof(messageType));
        Send(rootAddress, data);
        break;
    }
    case NOW_NO_INIT:
    case NOW_INIT:
    case MESH_ROOT:
    default:
        break;
    }
}

void EspmeshManager::Send(const MacAddress address, const Payload data)
{
    driver.Send(address, data);
}

void EspmeshManager::SendBroadcast(const Payload data)
{
    driver.SendBroadcast(data);
}

void EspmeshManager::Receive(const MacAddress address, const Payload data)
{
    Payload payloadMessage = Payload(data);
    Payload payloadApplicationIdentifier = Payload(sizeof(EspMeshApplication));
    Payload payloadMessageIdentifier = Payload(sizeof(EspMeshMessageType));
    Payload payloadData = Payload(payloadMessage.GetSize() - (payloadApplicationIdentifier.GetSize() + payloadMessageIdentifier.GetSize()));

    payloadMessage >>= payloadData;
    payloadMessage >>= payloadMessageIdentifier;
    payloadMessage >>= payloadApplicationIdentifier;

    EspMeshMessageType messageType;
    EspMeshApplication application;
    std::memcpy(&application, payloadApplicationIdentifier.data, sizeof(messageType));
    std::memcpy(&messageType, payloadMessageIdentifier.data, sizeof(messageType));

    if(application != MESH)
    {
        return;
    }

    switch (messageType)
    {
    case MESH_ROOT_UPDATED:
    {
        bool isRoot;
        std::memcpy(&isRoot, payloadMessage.data, sizeof(isRoot));
        if(isRoot)
        {
            internalState = MESH_ROOT;
        }
        else
        {
            internalState = MESH_NON_ROOT;
        }
        rootAddress = address;
        std::string textMessage = "Root address acquired. ";
        if(isRoot)
        {
            textMessage += "We are root.";
        }
        else
        {
            textMessage += "Root is ";
            textMessage += rootAddress.ToString();
        }
        logManager.Log(W, "EspmeshDriver", textMessage.c_str());

        break;
    }
    case MESH_KEEP_ALIVE:
        logManager.Log(W, "EspmeshManager", "MESH_KEEP_ALIVE received\n");
    default:
        break;
    }
}

std::string EspmeshManager::GetMonitorData()
{
    return "EspmeshManager\n";
}
