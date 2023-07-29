#ifndef TaskManager_CFG_H
#define TaskManager_CFG_H

#include "TaskManager.h"
#include "WifiManager.h"
#include "EspnowManager.h"

typedef struct Init_cfg_struct{
    const char* name;
    void (*InitFunction)(void*);
}Init_cfg_struct;

Init_cfg_struct init_cfg[] =
{ 
    {"WifiManager", WifiManager_Init},
    {"EspnowManager", EspnowManager_Init}
};

#define Init_cfg_size sizeof(init_cfg)/sizeof(Init_cfg_struct)
Task_cfg_struct task_cfg[] =
{
    EspnowManager_MainFunction_Config
};

#define Task_cfg_size sizeof(task_cfg)/sizeof(Task_cfg_struct)


#endif