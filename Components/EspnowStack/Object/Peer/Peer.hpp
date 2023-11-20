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
        static const uint8 seriesBeginningLife = 5;
        Spinlock selfProtection = Spinlock_Init;
        Distance distance;
        uint8 sourceAddress[6];
        std::vector<SeriesLife> openSeries;
        Spinlock openSeriesProtection = Spinlock_Init;
        bool isPeerSubscribedToUs   = false;
        bool areWeSubscribedToPeer  = false;
        bool acknowledgeRequired    = false;
    public:
        Peer(const uint8_t*);
        ~Peer();
        bool IsCorrectAddress(const uint8*);
        void RSSI_Msg_Received(RSSI_Message_Request     message);
        void RSSI_Msg_Received(RSSI_Message_Calculation message);
        void RSSI_Msg_Received(RSSI_Message_Keep_Alive  message);
        void RSSI_Msg_Received(RSSI_Message_Acknowledge message);
        void UpdateSeries();
        void SendSubscriptionRequest();
        void Monitor();
};

#endif