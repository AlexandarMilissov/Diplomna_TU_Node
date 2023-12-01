#include "Hooks.h"
#include "esp_freertos_hooks.h"
#include "TaskManager.h"

void Hooks_Init(const void* pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

    LogManager_SetMinimalLevel("Hooks", W);

    esp_register_freertos_idle_hook_for_cpu(&Idle_Hook_0, CORE_0);
    esp_register_freertos_idle_hook_for_cpu(&Idle_Hook_1, CORE_1);
}

bool Idle_Hook_0()
{
    /**
     * The callback should return true if it should be called by the idle hook once per interrupt (or FreeRTOS tick),
     * and return false if it should be called repeatedly as fast as possible by the idle hook.
    */
    return true;
}

bool Idle_Hook_1()
{
    /**
     * The callback should return true if it should be called by the idle hook once per interrupt (or FreeRTOS tick),
     * and return false if it should be called repeatedly as fast as possible by the idle hook.
    */
    return true;
}
