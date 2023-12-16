#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <vector>
#include "Common.hpp"
#include "TaskNameTable.h"


// Define the log function signature
typedef const char* (*LogFunctionSignature)(void);


class Monitor
{
private:
    static std::vector<LogFunctionSignature> logDelegate;
    static size_t maximum_heap_regions;
    static size_t free_heap_regions;
    static const char *MonitorMemory();
    static const char *MonitorCPU();
public:
    static void Init(const void*);
    static void MainFunction(const void*);
    static void SubscribeFunction(LogFunctionSignature);
    static void UnsubscribeFunction(LogFunctionSignature);
};

#define Monitor_MainFunction_Config                                                     \
{                                                                                       \
    MonitorName,                    /* The name of task                         */      \
    Monitor::MainFunction,          /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    2000,                           /* Period of the cyclic function            */      \
    CORE_1,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    21,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#endif // MONITOR_HPP_