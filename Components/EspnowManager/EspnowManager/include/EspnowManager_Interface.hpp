#ifndef ESPNOWMANAGER_INTERFACE_H
#define ESPNOWMANAGER_INTERFACE_H

#include "Common.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

void EspnowManager_ActivateNetwork();
void EspnowManager_DeactivateNetwork();
void EspnowManager_Subscribe();
void EspnowManager_Unsubscribe();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
