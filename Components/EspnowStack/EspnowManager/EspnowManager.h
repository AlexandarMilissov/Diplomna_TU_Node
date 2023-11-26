#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <esp_now.h>
#include "EspnowManager_Task_Config.h"
#include "Common.h"
#include <stdatomic.h>

typedef enum
{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}State;

void EspnowManager_Init(const void*);

void EspnowManager_MainFunctionUpdatePeers              (const void* pvParameters);
void EspnowManager_MainFunction_Send_Cyclic_KeepAlive   (const void* pvParameters);
void EspnowManager_MainFunction_Send_Cyclic_Calculation (const void* pvParameters);
void EspnowManager_MainFunction_HandleReceivedMessages  (const void* pvParameters);
void EspnowManager_ActivateNetwork();
void EspnowManager_DeactivateNetwork();
void AddCalculationSubscriber();
void RemoveCalculationSubscriber();

#endif
