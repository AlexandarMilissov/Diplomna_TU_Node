#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <esp_now.h>
#include "Common.h"

void EspnowManager_Init(void*);
void EspnowManager_MainFunction(void*);
void Request_RSSI_Calculation_Chain(uint8*);

#endif