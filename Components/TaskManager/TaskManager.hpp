#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include "Common.hpp"

typedef struct Task_cfg_struct{
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
}Task_cfg_struct;

class TaskManager
{
private:
    const static uint16 wdtTimeout = 5000;
    static void Task(void*);
    static bool IsTaskCfgValid(Task_cfg_struct config);
    /* data */
public:
    static void Init(const void*);
    static TaskHandle_t* RequestTask(Task_cfg_struct* config);
};


#endif