#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus

#include "Common.h"
#include "TaskNameTable.h"

#define Monitor_MainFunction_Config                                                     \
{   {0},                                                                                    \
    MonitorName,                    /* The name of task                         */      \
    Monitor_MainFunction,           /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    2000,                           /* Period of the cyclic function           */      \
    CORE_1,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    210,                            /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL,{0}                            /* Parameters for the onComplete function   */      \
}
void Monitor_Init(const void*);
void Monitor_MainFunction(const void*);

void MonitorTaskInit(uint8, const uint16);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MONITOR_HPP_