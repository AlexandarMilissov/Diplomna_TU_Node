#ifndef TaskManager_CFG_H
#define TaskManager_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "TaskManager.h"
#include "WifiManager.h"
#include "EspnowManager_Tasks.hpp"
#include "LogManager.h"
#include "Monitor.hpp"
#include "Hooks.h"
#include "NvsManager.h"

typedef struct Init_cfg_struct{
    const char* name;
    void (*InitFunction)(const void*);
}Init_cfg_struct;

Init_cfg_struct init_cfg[] =
{
    {"LogManager",          LogManager_Init},
    {"NvsManager",          NvsManager_Init},
#if CONFIG_ENABLE_MONITOR == 1
    {"Monitor",             Monitor_Init},
#endif
    {"WifiManager",         WifiManager_Init},
    {"EspnowManager",       EspnowManager_Init},
    {"Hooks",               Hooks_Init},
};

#define Init_cfg_size sizeof(init_cfg)/sizeof(Init_cfg_struct)
Task_cfg_struct task_cfg[] =
{
    EspnowManager_MainFunctionUpdatePeers_Config,
    EspnowManager_MainFunction_Send_Cyclic_KeepAlive_Config,
    EspnowManager_MainFunction_Send_Cyclic_Calculation_Config,
    EspnowManager_MainFunction_HandleReceivedMessages_Core0_Config,
    EspnowManager_MainFunction_HandleReceivedMessages_Core1_Config,

#if CONFIG_ENABLE_MONITOR == 1
    Monitor_MainFunction_Config
#endif
};

#define Task_cfg_size sizeof(task_cfg)/sizeof(Task_cfg_struct)

#ifdef __cplusplus
}
#endif //__cplusplus

#endif