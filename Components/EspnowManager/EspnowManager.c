#include "Common.h"
#include "EspnowManager.h"
#include "EspnowManager_Private.h"
#include "RSSI_Calculation.h"
#include "Debug_Txt.h"
#include "TaskManager.h"

State state = NO_INIT;
uint8 myID = 2;

void EspnowManager_Init(void* pvParameters)
{
    state = INIT;
    espnow_storage_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, DataReceive);

    RSSI_Calculation_Init(NULL);
    state = RUN;
}

void EspnowManager_MainFunction(void* pvParameters)
{
    RSSI_Keep_Alive_Msg_Send(NULL);
}

void Request_RSSI_Calculation_Chain(uint8* address)
{
    Task_cfg_struct task_config = {
        .name = "RSSI_Calculation_Msg_Send",
        .MainFunction = RSSI_Calculation_Msg_Send,
        .mainFunctionParams = (void*)address,
        .period = 20,
        .core = -1,
        .stack_size = 4096,
        .priority = 100,
        .finite = true,
        .repetition = NUM_OF_KEPT_RECEIVED,
        .OnComplete = RSSI_Acknowledge_Msg_Send,
        .onCompleteParams = (void*)SENDEND
    };
    RequestTask(task_config);
}