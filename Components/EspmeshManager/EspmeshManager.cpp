#include "EspmeshManager.hpp"
#include "EspnowManager.hpp"
#include "Common.hpp"

void EspmeshManager::Init()
{
    driver.Subscribe(*this);
    espnowController.ActivateNetwork();

    TaskConfig config = TaskConfig(
        "EspmeshManager",
        [this]() { MainFunction(); },
        1000,
        CORE_1,
        8192,
        10
    );
    taskManager.RequestTask(config);
}

void EspmeshManager::MainFunction()
{
    espnowController.ActivateNetwork();
}

void EspmeshManager::Send(const Payload payload, const Payload metadata)
{

}

void EspmeshManager::Receive(const Payload* payload, const Payload* metadata)
{

}

std::string EspmeshManager::GetMonitorData()
{
    return "EspmeshManager";
}
