#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <esp_now.h>
#include "Common.h"

#define EspnowManager_MainFunction_Config                                             \
{                                                                                     \
    "EspnowManager_MainFunction",   /* The name of task                         */    \
    EspnowManager_MainFunction,     /* The cyclic function the task calls       */    \
    NULL,                           /* Parameters for the cyclic function       */    \
    50,                             /* Perieod of the cyclic function           */    \
    CORE_0,                         /* Id of the core                           */    \
    8192,                           /* Task stack size                          */    \
    200,                            /* Task priority                            */    \
    false,                          /* Is the task finite                       */    \
    0,                              /* Number of repetitions for finite task    */    \
    NULL,                           /* Function to call when tasks finishes     */    \
    NULL                            /* Parameters for the onComplete function   */    \
}

typedef enum State{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}State;

void EspnowManager_Init(void*);
void EspnowManager_MainFunction(void*);
void EspnowManager_ActivateNetwork();
void EspnowManager_DeactivateNetwork();
void AddCalculationSubsciber();
void RemoveCalculationSubsciber();

#endif
