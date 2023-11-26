#ifndef PEER_H
#define PEER_H

#include "Common.h"
#include "Distance.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Acknowledge.hpp"

#include <list>
#include <atomic>

typedef struct SeriesLife
{
    OpenSeries* series;
    uint8 life;
}SeriesLife;

class Peer
{
    private:
        static const uint8 seriesBeginningLife = 5;
        static const uint8 peerBeginningLife = 5;
        std::atomic<uint8> peerLife;
        Distance distance;
        uint8 sourceAddress[6];
        std::vector<SeriesLife> openSeries;
        Spinlock subscriptionStateProtection = Spinlock_Init;
        Spinlock calculationDataProtection = Spinlock_Init;
        std::atomic<bool> isPeerSubscribedToUs   = false;
        std::atomic<bool> areWeSubscribedToPeer  = false;
        std::atomic<bool> acknowledgeRequired    = false;
    public:
        Peer(const uint8_t*);
        ~Peer();
        bool IsCorrectAddress(const uint8*);
        void RSSI_Msg_Received(RSSI_Message_Request     message);
        void RSSI_Msg_Received(RSSI_Message_Calculation message);
        void RSSI_Msg_Received(RSSI_Message_Keep_Alive  message);
        void RSSI_Msg_Received(RSSI_Message_Acknowledge message);
        bool IsAlive();
        void Refresh();
        void SendSubscriptionRequest();

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR
        const char* Log();
#endif
};

#endif