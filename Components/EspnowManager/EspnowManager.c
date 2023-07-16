#include "Common.h"
#include "EspnowManager.h"
#include "Espnow_Message_General.h"
#include "Debug_Txt.h"
#include "TaskManager.h"
#include "To_CPP_Encapsulation.hpp"

State state = NO_INIT;
uint8 myID = 2;

#define RSSI_REF_AT_1M -50
// -50 at 1 m should be -90 at 100m
// https://www.changpuak.ch/electronics/calc_10.php
// Antenna gain -30
#define RSSI_N 2

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
    if(RUN == state)
    {
        Send_Keep_Alive_Msg();
    }
}

void EspnowManager_ActivateNetwork()
{
    state = RUN;
}
