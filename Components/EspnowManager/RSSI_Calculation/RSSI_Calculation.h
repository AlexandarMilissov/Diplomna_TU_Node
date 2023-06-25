#ifndef RSSI_CALCULATION_H
#define RSSI_CALCULATION_H

#include "Common.h"
#include "EspnowManager_Private.h"
#define NUM_OF_KEPT_RECEIVED 15

typedef signed int RSSI_Type;

typedef struct PeerHistory
{
    RSSI_Type RSSI;
    uint64 time;
}PeerHistory;

typedef struct Peer{
    RSSI_Type RSSI;
    double RSSI_average;
    RSSI_Type lastReceived[NUM_OF_KEPT_RECEIVED];
    uint16 received_counter;
    uint8 sourceAddress[6];
    List_I* history;
}Peer;

typedef struct RSSI_Request_Msg{
    uint8 info;
}RSSI_Request_Msg;

typedef struct RSSI_Calculation_Msg
{
    uint16 id;
}RSSI_Calculation_Msg;

typedef struct RSSI_Keep_Alive_Msg{
    uint8 info;
}RSSI_Keep_Alive_Msg;

enum RSSI_Acknowledge_Status{
    SENDEND,
    REJECT
};
typedef struct RSSI_Acknowledge_Msg{
    enum RSSI_Acknowledge_Status status;
}RSSI_Acknowledge_Msg;

void RSSI_Calculation_Init(void*);
void RSSI_Common_Received(uint8_t*, Message, size_t, wifi_pkt_rx_ctrl_t*);

void RSSI_Request_Msg_Received(Peer*, RSSI_Request_Msg, uint8*);
void RSSI_Calculation_Msg_Received(Peer*, RSSI_Calculation_Msg, RSSI_Type, uint8*);
void RSSI_Keep_Alive_Msg_Received(Peer*, RSSI_Keep_Alive_Msg, RSSI_Type, uint8*);
void RSSI_Acknowledge_Msg_Received(Peer*, RSSI_Acknowledge_Msg, uint8*);

void RSSI_Request_Msg_Send(void*);
void RSSI_Calculation_Msg_Send(void*);
void RSSI_Acknowledge_Msg_Send(void*);
void RSSI_Keep_Alive_Msg_Send(void*);

#endif