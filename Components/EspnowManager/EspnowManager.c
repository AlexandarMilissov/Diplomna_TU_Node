#include "EspnowManager.h"
#include "Common.h"
#include "EspnowManager.h"
#include "EspnowManager_Private.h"
#include "RSSI_Calculation.h"
#include "Debug_Txt.h"

#include "espnow.h"
#include "espnow_utils.h"
#include "espnow_storage.h"

State state = NO_INIT;

void EspnowManager_Init(void* pvParameters)
{
    espnow_storage_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, DataReceive);

    RSSI_Calculation_Init(NULL);
    state = INIT;
}

uint32 counter = 0;
void EspnowManager_MainFunction(void* pvParameters)
{
    RSSI_Calculation_Msg data = {
        .id = 2,
    };
    Message RSSIMessage = {
        .messageCouter = counter,
        .messageId = RSSI_CALCULATION,
        .messageSize = sizeof(RSSI_Calculation_Msg),
        .message = (void*)(&data),
    };
    DataSend((uint8_t*)ESPNOW_ADDR_BROADCAST, RSSIMessage);

    counter++;
}
