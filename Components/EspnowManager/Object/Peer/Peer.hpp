#ifndef PEER_H
#define PEER_H

#include "Common.h"
#include "Distance.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Acknowledge.hpp"


class Peer
{
    private:
        Distance distance;
        uint8 sourceAddress[6];
        static void CalculationChain_MainFunction(void*);
        static void CalculationChain_EndFunction(void*);
    public:
        Peer(uint8_t*);
        ~Peer();
        bool IsCorrectAdress(uint8*);
        void RSSI_Msg_Received(RSSI_Message_Request     message);
        void RSSI_Msg_Received(RSSI_Message_Calculation message);
        void RSSI_Msg_Received(RSSI_Message_Keep_Alive  message);
        void RSSI_Msg_Received(RSSI_Message_Acknowledge message);
};

#endif