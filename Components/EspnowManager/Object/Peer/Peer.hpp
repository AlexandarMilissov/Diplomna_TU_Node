#ifndef PEER_H
#define PEER_H

#include "Common.h"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Acknowledge.hpp"

#define NUM_OF_KEPT_RECEIVED 15

typedef enum CalculationStatus
{
    CALCULATION_INIT,
    CALCULATED_POST_INIT,
}CalculationStatus;

typedef enum PeerConnection
{
    CONNECTION_OK,
    CONNECTION_NOT_OK
}PeerConnection;

class Peer
{
    private:
        RSSI_Type RSSI;
        double RSSI_average;
        RSSI_Type lastReceived[NUM_OF_KEPT_RECEIVED];
        uint16 received_counter;
        uint8 sourceAddress[6];
        CalculationStatus calculationStatus = CALCULATION_INIT;
        static void CalculationChain_MainFunction(void*);
        static void CalculationChain_EndFunction(void*);
        void Recalculate();
        PeerConnection EvaluateConnection();
    public:
        Peer(uint8_t*);
        bool IsCorrectAdress(uint8*);
        void RSSI_Msg_Received(RSSI_Message_Request     message);
        void RSSI_Msg_Received(RSSI_Message_Calculation message);
        void RSSI_Msg_Received(RSSI_Message_Keep_Alive  message);
        void RSSI_Msg_Received(RSSI_Message_Acknowledge message);
};

#endif