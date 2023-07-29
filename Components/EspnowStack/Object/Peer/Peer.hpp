#ifndef PEER_H
#define PEER_H

#include "Common.h"
#include "Distance.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Acknowledge.hpp"

#include <list>

typedef struct SeriesLife
{
    OpenSeries* series;
    uint8 life;
}SeriesLife;

class Peer
{
    private:
        static const uint8 seriersBeginningLife = 5;
        Distance distance;
        uint8 sourceAddress[6];
        std::vector<SeriesLife> openSeries;
        bool isPeerSubscirbedToUs   = false;
        bool areWeSubscirbedToPeer  = false;
        bool acknowledgeRequired    = false;
    public:
        Peer(uint8_t*);
        ~Peer();
        bool IsCorrectAdress(uint8*);
        void RSSI_Msg_Received(RSSI_Message_Request     message);
        void RSSI_Msg_Received(RSSI_Message_Calculation message);
        void RSSI_Msg_Received(RSSI_Message_Keep_Alive  message);
        void RSSI_Msg_Received(RSSI_Message_Acknowledge message);
        void UpdateSeries();
        void SendSubsriptionRequest();
};

#endif