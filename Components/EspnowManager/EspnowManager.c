#include "Common.h"
#include "EspnowManager.h"
#include "Espnow_Message_General.h"
#include "Debug_Txt.h"
#include "TaskManager.h"
#include "To_CPP_Encapsulation.hpp"

State state = NO_INIT;
uint8 myID = 2;
uint16 calculationSubscibers = 0;

void EspnowManager_Init(void* pvParameters)
{
    espnow_storage_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, DataReceive);

    To_CPP_Encapsulation_Init(NULL);

    state = INIT;

    EspnowManager_ActivateNetwork();
}

void EspnowManager_MainFunction(void* pvParameters)
{
    if(RUN != state)
    {
        return;
    }
    HandleReceivedMessages();

    if(0 < calculationSubscibers)
    {
        //calculationSubscibers--;
        Task_cfg_struct task_cfg = {
            .core = CORE_1,
            .finite = true,
            .repetition = 10,
            .MainFunction = SeriesSend,
            .name = "Series Task",
            .OnComplete = NULL,
            .onCompleteParams = NULL,
            .period = 3,
            .priority = 200,
            .stack_size = 8192,
        };
        RequestTask(task_cfg);
    }
    Send_Cyclic_Msg();

    UpdateSeries();
}
void EspnowManager_ActivateNetwork()
{
    state = RUN;
}

void EspnowManager_DeactivateNetwork()
{
    state = INIT;
}

void AddCalculationSubsciber()
{
    calculationSubscibers++;
    printf("Subscibers: %d\n", calculationSubscibers);
}

void RemoveCalculationSubsciber()
{
    calculationSubscibers--;
    printf("Subscibers: %d\n", calculationSubscibers);
}