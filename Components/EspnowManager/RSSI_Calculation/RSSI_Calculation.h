#ifndef RSSI_CALCULATION_H
#define RSSI_CALCULATION_H

#include "Common.h"

#define NUM_OF_KEPT_RECEIVED 5

typedef signed int RSSI_Type;

typedef struct Peer{
    uint16 id;
    RSSI_Type RSSI;
    RSSI_Type lastReceived[NUM_OF_KEPT_RECEIVED];
    uint16 received_counter;
}Peer;

typedef struct RSSI_Calculation_Msg
{
    uint16 id;
}RSSI_Calculation_Msg;

void RSSI_Calculation_Init(void*);
void RSSI_Calculation_Msg_Received(void*, size_t, RSSI_Type);

#endif