#include "Hooks.hpp"
#include "esp_freertos_hooks.h"

void Hooks::Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

    LogManager::SetMinimalLevel("Hooks", W);

    esp_register_freertos_idle_hook_for_cpu(&Hooks::Idle_0, CORE_0);
    esp_register_freertos_idle_hook_for_cpu(&Hooks::Idle_1, CORE_1);
}

bool Hooks::Idle_0()
{
    /**
     * The callback should return true if it should be called by the idle hook once per interrupt (or FreeRTOS tick),
     * and return false if it should be called repeatedly as fast as possible by the idle hook.
    */
    return true;
}

bool Hooks::Idle_1()
{
    /**
     * The callback should return true if it should be called by the idle hook once per interrupt (or FreeRTOS tick),
     * and return false if it should be called repeatedly as fast as possible by the idle hook.
    */
    return true;
}
