#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <esp_now.h>
#include "Common.h"
                 
typedef enum State{
    NO_INIT,
    INIT,
    RUN,
}State;

void EspnowManager_Init(void*);
void EspnowManager_MainFunction(void*);

#endif