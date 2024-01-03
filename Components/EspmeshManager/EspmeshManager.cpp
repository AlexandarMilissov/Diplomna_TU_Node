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
        Payload address = Payload(rootAddress, sizeof(rootAddress));
        EspMeshMessageType enumData = MESH_KEEP_ALIVE;
        Payload data = Payload((uint8*)(&enumData), sizeof(enumData));
        Send(address, data);
        break;
    }
    case NOW_NO_INIT:
    case NOW_INIT:
    case MESH_ROOT:
    default:
        break;
    }
}

void EspmeshManager::Send(const Payload address, const Payload data)
{
    driver.Send(address, data);
}

void EspmeshManager::Receive(const Payload* address, const Payload* data)
{
    Payload message_identifier = Payload(sizeof(EspMeshMessageType));
    Payload message_data = Payload(*(data));
    message_data >>= message_identifier;

    EspMeshMessageType enumData;
    std::memcpy(&enumData, message_identifier.data, sizeof(enumData));

    switch (enumData)
    {
    case MESH_ROOT_UPDATED:
    {
        bool isRoot;
        std::memcpy(&isRoot, message_data.data, sizeof(isRoot));
        if(isRoot)
        {
            internalState = MESH_ROOT;
        }
        else
        {
            internalState = MESH_NON_ROOT;
        }
        std::memcpy(rootAddress, address->data, sizeof(rootAddress));
        std::string textMessage;
        if(isRoot)
        {
            textMessage = "Root address acquired. We are root.";
        }
        else
        {
            textMessage = "Root address acquired. Root is ";
            textMessage +=
                std::to_string(rootAddress[0]) + ":" +
                std::to_string(rootAddress[1]) + ":" +
                std::to_string(rootAddress[2]) + ":" +
                std::to_string(rootAddress[3]) + ":" +
                std::to_string(rootAddress[4]) + ":" +
                std::to_string(rootAddress[5]);
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
