#include "EspmeshManager.hpp"
#include "EspnowManager.hpp"
#include "Common.hpp"

void EspmeshManager::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);
}

void EspmeshManager::MainFunction(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

#if 1
    EspnowManager::ActivateNetwork();
#endif

}
