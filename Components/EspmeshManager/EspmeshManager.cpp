#include "EspmeshManager.hpp"
#include "EspnowManager.hpp"
#include "Common.hpp"

void EspmeshManager::Init()
{
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
