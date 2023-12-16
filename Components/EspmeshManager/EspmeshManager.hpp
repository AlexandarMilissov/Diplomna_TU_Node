#ifndef ESPMESHMANAGER_HPP_
#define ESPMESHMANAGER_HPP_

#define EspmeshManager_MainFunction_Config                                              \
{                                                                                       \
    MonitorName,                    /* The name of task                         */      \
    EspmeshManager::MainFunction,   /* The cyclic function the task calls       */      \
    NULL,                           /* Parameters for the cyclic function       */      \
    1000,                           /* Period of the cyclic function            */      \
    CORE_1,                         /* Id of the core                           */      \
    8192,                           /* Task stack size                          */      \
    20,                             /* Task priority                            */      \
    false,                          /* Is the task finite                       */      \
    0,                              /* Number of repetitions for finite task    */      \
    NULL,                           /* Function to call when tasks finishes     */      \
    NULL                            /* Parameters for the onComplete function   */      \
}

class EspmeshManager
{
private:
    /* data */
public:
    static void Init(const void*);
    static void MainFunction(const void*);
};

#endif // ESPMESHMANAGER_HPP_
