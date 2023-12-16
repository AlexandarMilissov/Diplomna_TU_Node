#ifndef TaskManager_CFG_HPP
#define TaskManager_CFG_HPP

#include "TaskManager.hpp"
#include "WifiManager.hpp"
#include "LogManager.hpp"
#include "Monitor.hpp"
#include "Hooks.hpp"
#include "NvsManager.hpp"
#include "EspnowManager.hpp"
#include "EspnowManager_Task_Config.h"

typedef struct Init_cfg_struct{
    const char* name;
    void (*InitFunction)(const void*);
}Init_cfg_struct;

Init_cfg_struct init_cfg[] =
{
    {"LogManager",          LogManager::Init},
    {"NvsManager",          NvsManager::Init},
#if CONFIG_ENABLE_MONITOR == 1
    {"Monitor",             Monitor::Init},
#endif
    {"WifiManager",         WifiManager::Init},
    {"EspnowManager",       EspnowManager::Init},
    {"Hooks",               Hooks::Init},
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

#define Init_cfg_size sizeof(init_cfg)/sizeof(Init_cfg_struct)
#define Task_cfg_size sizeof(task_cfg)/sizeof(Task_cfg_struct)

#endif
