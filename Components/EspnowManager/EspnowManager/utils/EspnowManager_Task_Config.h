#ifndef ESPNOWMANAGER_TASK_CONFIG_H_
#define ESPNOWMANAGER_TASK_CONFIG_H_

#include "TaskNameTable.h"
#include "EspnowManager_Config_Validation.h"

#define EspnowManager_MainFunctionUpdatePeers_Config                                    \
{                                                                                       \
    UpdateSeriesName,               /* The name of task                         */      \
    EspnowManager::MainFunctionUpdatePeers,                                             \
                                    /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    300,                            /* Period of the cyclic function            */      \
    CORE_0,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#define EspnowManager_MainFunction_Send_Cyclic_KeepAlive_Config                         \
{                                                                                       \
    SendCyclicKeepAliveName,        /* The name of task                         */      \
    EspnowManager::MainFunctionSendCyclicKeepAlive,                                     \
                                    /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    250,                            /* Period of the cyclic function            */      \
    CORE_0,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#define EspnowManager_MainFunction_Send_Cyclic_Calculation_Config                       \
{                                                                                       \
    SendCyclicCalculationName,      /* The name of task                         */      \
    EspnowManager::MainFunctionSendCyclicCalculation,                                   \
                                    /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    CONFIG_SERIES_INITIATION_INTERVAL,                                                  \
                                    /* Period of the cyclic function            */      \
    CORE_0,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#define EspnowManager_MainFunction_HandleReceivedMessages_Core0_Config                  \
{                                                                                       \
    HandleReceivedMessagesOnCore0Name,                                                  \
                                    /* The name of task                         */      \
    EspnowManager::MainFunctionHandleReceivedMessages,                                  \
                                    /* The cyclic function the task calls       */      \
    (void*)CORE_0,                  /* Parameters for the cyclic function       */      \
    50,                             /* Period of the cyclic function            */      \
    CORE_0,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#define EspnowManager_MainFunction_HandleReceivedMessages_Core1_Config                  \
{                                                                                       \
    HandleReceivedMessagesOnCore1Name,                                                  \
                                    /* The name of task                         */      \
    EspnowManager::MainFunctionHandleReceivedMessages,                                  \
                                    /* The cyclic function the task calls       */      \
    (void*)CORE_1,                  /* Parameters for the cyclic function       */      \
    50,                             /* Period of the cyclic function            */      \
    CORE_0,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

#define EspnowManager_SendCalculationFunction_Config                                    \
{                                                                                       \
    SendCalculationSeriesName,      /* The name of task                         */      \
    EspnowManager::SendCalculationSeries,                                                \
                                    /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    CONFIG_SERIES_CYCLIC_SEND_INTERVAL,                                                 \
                                    /* Period of the cyclic function            */      \
    CORE_1,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    10,                             /* Task priority                            */      \
    true,                           /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}


#endif // ESPNOWMANAGER_TASK_CONFIG_H_
