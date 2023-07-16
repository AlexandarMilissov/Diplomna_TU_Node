#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <esp_now.h>
#include "Common.h"
                 
typedef enum State{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}State;

void EspnowManager_Init(void*);
void EspnowManager_MainFunction(void*);
void EspnowManager_ActivateNetwork();

#endif