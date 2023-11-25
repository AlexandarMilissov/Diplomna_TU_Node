#ifndef CMC_H
#define CMC_H

#include "Common.h"

#define CORE_0 0
#define CORE_1 1

typedef struct Task_cfg_struct{
    uint8 padding1[100];
    const uint8 namePointer;
    void (*MainFunction)(const void*);
    void* mainFunctionParams;
    uint16 period;
    uint8 core;
    uint32 stack_size;
    UBaseType_t priority;
    bool finite;
    uint16 repetition;
    void (*OnComplete)(const void*);
    void* onCompleteParams;
    uint8 padding2[100];
}Task_cfg_struct;

void TaskManager_Init(void);
TaskHandle_t* RequestTask(Task_cfg_struct config);

#endif