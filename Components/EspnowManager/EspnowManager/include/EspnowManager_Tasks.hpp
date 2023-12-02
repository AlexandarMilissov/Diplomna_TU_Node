#ifndef ESPNOWMANAGER_TASKS_H_
#define ESPNOWMANAGER_TASKS_H_


#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "../utils/EspnowManager_Task_Config.h"

void EspnowManager_Init                                 (const void*);

void EspnowManager_MainFunctionUpdatePeers              (const void*);
void EspnowManager_MainFunction_Send_Cyclic_KeepAlive   (const void*);
void EspnowManager_MainFunction_Send_Cyclic_Calculation (const void*);
void EspnowManager_MainFunction_HandleReceivedMessages  (const void*);

void EspnowManager_SendCalculationSeries                (const void*);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ESPNOWMANAGER_TASKS_H_