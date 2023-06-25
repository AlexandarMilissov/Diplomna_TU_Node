#include "RSSI_Calculation.h"
#include "EspnowManager_Private.h"
#include "EspnowManager.h"
#include "Lists.h"

#include <math.h>
#include "esp_timer.h"

#define RSSI_REF_AT_1M -50
// -50 at 1 m should be -90 at 100m
// https://www.changpuak.ch/electronics/calc_10.php
// Antenna gain -30
#define RSSI_N 2

List_I* Peers;
uint32 counter = 0;

extern uint8 myID;
extern RSSI_Type RSSI_Deviation;

bool isRequestOnGoing = false;
Peer* FindPeerInList(uint8*);
Peer* NewPeer();
void RecalculateRSSI(Peer*);
float DistanceCalculation(RSSI_Type);

void RSSI_Calculation_Init(void* pvparams)
{
    Peers = NewList();
}

void RSSI_Common_Received(uint8_t* src_addr, Message message, size_t size, wifi_pkt_rx_ctrl_t* rx_ctrl)
{
    First(Peers);
    Peer* peer = FindPeerInList(src_addr);
    
    if(NULL == peer)
    {
        peer = NewPeer(src_addr);

        Last(Peers);
        Add(Peers, peer);
        espnow_add_peer(src_addr, NULL);
        
        Request_RSSI_Calculation_Chain(peer->sourceAddress);
    }

    switch (message.messageId)
    {
    case RSSI_REQUEST:
    {
        if(sizeof(RSSI_Request_Msg) == message.messageSize)
        {
            RSSI_Request_Msg_Received(peer, *(RSSI_Request_Msg*)message.message, src_addr);
        }
    }
        break;
    case RSSI_CALCULATION:
    {
        if(sizeof(RSSI_Calculation_Msg) == message.messageSize)
        {
            RSSI_Calculation_Msg_Received(peer, *(RSSI_Calculation_Msg*)message.message, rx_ctrl->rssi, src_addr);
        }
    }
        break;
    case RSSI_KEEP_ALIVE:
    {
        if(sizeof(RSSI_Keep_Alive_Msg) == message.messageSize)
        {
            RSSI_Keep_Alive_Msg_Received(peer, *(RSSI_Keep_Alive_Msg*)message.message, rx_ctrl->rssi, src_addr);
        }
    }
        break;
    case RSSI_ACKNOWLEDGE:
    {
        if(sizeof(RSSI_Acknowledge_Msg) == message.messageSize)
        {
            RSSI_Acknowledge_Msg_Received(peer, *(RSSI_Acknowledge_Msg*)message.message, src_addr);
        }
    }
        break;
    }
}

void RecalculateRSSI(Peer* peer)
{
    PeerHistory* peerHistory = (PeerHistory*)malloc(sizeof(PeerHistory));
    peerHistory->RSSI = peer->RSSI;
    peerHistory->time = esp_timer_get_time();
    Last(peer->history);
    Add(peer->history, peerHistory);

    RSSI_Type newRSSI = 0;
    for(int i = 0; i < NUM_OF_KEPT_RECEIVED; i++)
    {
        newRSSI += peer->lastReceived[i];
    }
    peer->RSSI = newRSSI / NUM_OF_KEPT_RECEIVED;
    peer->RSSI_average = ((double)newRSSI) / NUM_OF_KEPT_RECEIVED;
    for(int i = 0; i < NUM_OF_KEPT_RECEIVED; i++)
    {
        printf("%d ", peer->lastReceived[i]);
    }
    printf("\nNew RSSI: avrg:%f, count:%d\n", peer->RSSI_average, peer->received_counter);
    printf("Distance: %f\n", DistanceCalculation(peer->RSSI_average));
}

Peer* FindPeerInList(uint8 address[6])
{
    Peer* peer = NULL;
    First(Peers);
    if(Peers->count > 0)
    {
        for(int i = 0; i < Peers->count; i++)
        {
            if(0 == memcmp(((Peer*)This(Peers))->sourceAddress, address, 6))
            {
                peer = ((Peer*)This(Peers));
                break;
            }
            else
            {
                Next(Peers);
            }
        }
    }
    return peer;
}

Peer* NewPeer(uint8_t* src_addr)
{
    Peer* peer = (Peer*)malloc(sizeof(Peer));

    peer->received_counter = 0;
    peer->RSSI = 0;
    memcpy(peer->sourceAddress, src_addr, 6);
    
    for(int i = 0; i < NUM_OF_KEPT_RECEIVED; i++)
    {
        peer->lastReceived[i] = 0;
    }

    peer->history = NewList();
    return peer;
}

float DistanceCalculation(RSSI_Type RSSI)
{
    return pow(10, ((RSSI_REF_AT_1M - (double)RSSI) / (10 * RSSI_N)));
}


void RSSI_Request_Msg_Received      (Peer* peer, RSSI_Request_Msg       message,                                uint8* sourceAddress)
{
    printf("RSSI_Request_Msg_Received\n");

    if(isRequestOnGoing)
    {
        enum RSSI_Acknowledge_Status sendStatus = REJECT;
        RSSI_Acknowledge_Msg_Send((void*)sendStatus);
    }
    else
    {
        isRequestOnGoing = true;
        Request_RSSI_Calculation_Chain(sourceAddress);
    }
}

void RSSI_Calculation_Msg_Received  (Peer* peer, RSSI_Calculation_Msg   message,    RSSI_Type Received_RSSI,    uint8* sourceAddress)
{
    printf("RSSI_Calculation_Msg_Received\n");

    peer->lastReceived[peer->received_counter % NUM_OF_KEPT_RECEIVED] = Received_RSSI;
    peer->received_counter++;
}

void RSSI_Keep_Alive_Msg_Received   (Peer* peer, RSSI_Keep_Alive_Msg    message,    RSSI_Type Received_RSSI,    uint8* sourceAddress)
{
    bool isCalculationNeeded = false;

    if(peer->received_counter < NUM_OF_KEPT_RECEIVED - 1)
    {
        isCalculationNeeded = true;
    }
    else if(peer->RSSI - RSSI_Deviation > Received_RSSI)
    {
        isCalculationNeeded = true;
    }

isCalculationNeeded = false;
    if(isCalculationNeeded)
    {
        RSSI_Request_Msg_Send(sourceAddress);
    }
}

void RSSI_Acknowledge_Msg_Received  (Peer* peer, RSSI_Acknowledge_Msg   message,                                uint8* sourceAddress)
{
    printf("RSSI_Acknowledge_Msg_Received\n");

    uint64 macVisualize = 0;
    memcpy(&macVisualize, peer->sourceAddress, 6);
    switch (message.status)
    {
    case SENDEND:
        RecalculateRSSI(peer);
        break;
    case REJECT:
        printf("Peer recalucaltion with %llx rejected.\n", macVisualize);
        break;
    default:
        printf("Wrong Acknowledge_Msg tag %d.\n", message.status);
        break;
    }
   
}



void RSSI_Request_Msg_Send      (void* vpmessage)
{
    printf("RSSI_Request_Msg_Send\n");
    RSSI_Request_Msg data = {
        .info = 0,
    };
    Message RSSIMessage = {
        .messageCouter = counter,
        .messageId = RSSI_REQUEST,
        .messageSize = sizeof(RSSI_Request_Msg),
        .message = (void*)(&data),
    };    
    counter++;

    DataSend((uint8_t*)vpmessage, RSSIMessage);
}

void RSSI_Calculation_Msg_Send  (void* vpmessage)
{
    printf("RSSI_Calculation_Msg_Send\n");
    RSSI_Calculation_Msg data = {
        .id = myID,
    };
    Message RSSIMessage = {
        .messageCouter = counter,
        .messageId = RSSI_CALCULATION,
        .messageSize = sizeof(RSSI_Calculation_Msg),
        .message = (void*)(&data),
    };    
    counter++;
    DataSend((uint8_t*)vpmessage, RSSIMessage);
}

void RSSI_Keep_Alive_Msg_Send   (void* vpmessage)
{
    RSSI_Keep_Alive_Msg data = {
           .info = counter % 30
    };
    Message RSSIMessage = {
        .messageCouter = counter,
        .messageId = RSSI_KEEP_ALIVE,
        .messageSize = sizeof(RSSI_Keep_Alive_Msg_Send),
        .message = (void*)(&data),
    };
    counter++;

    DataSend((uint8_t*)ESPNOW_ADDR_BROADCAST, RSSIMessage);
}

void RSSI_Acknowledge_Msg_Send  (void* vpmessage)
{
    printf("RSSI_Acknowledge_Msg_Send\n");

    if(SENDEND == (enum RSSI_Acknowledge_Status)vpmessage)
    {
        isRequestOnGoing = false;
    }

    RSSI_Acknowledge_Msg data = {
        .status = (enum RSSI_Acknowledge_Status)vpmessage,
    };
    Message RSSIMessage = {
        .messageCouter = counter,
        .messageId = RSSI_ACKNOWLEDGE,
        .messageSize = sizeof(RSSI_Acknowledge_Msg),
        .message = (void*)(&data),
    };
    counter++;

    DataSend((uint8_t*)ESPNOW_ADDR_BROADCAST, RSSIMessage);
}
