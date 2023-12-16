#ifndef PEER_H
#define PEER_H

#include "Common.hpp"
#include "Distance.hpp"
#include "EspnowMessageRequest.hpp"
#include "EspnowMessageCalculation.hpp"
#include "EspnowMessageKeepAlive.hpp"
#include "EspnowMessageAcknowledge.hpp"

#include <list>
#include <atomic>

typedef struct SeriesLife
{
    OpenSeries* series;
    uint8 life;
}SeriesLife;

class EspnowPeer
{
    private:
        static const uint8 seriesBeginningLife = 5;
        static const uint8 peerBeginningLife = 5;
        std::atomic<uint8> peerLife;
        Distance distance;
        uint8 sourceAddress[6];
        std::vector<SeriesLife> openSeries;
        Spinlock subscriptionStateLock = Spinlock_Init;
        Spinlock calculationDataLock = Spinlock_Init;
        std::atomic<bool> isPeerSubscribedToUs   = false;
        std::atomic<bool> areWeSubscribedToPeer  = false;
        std::atomic<bool> acknowledgeRequired    = false;
    public:
        EspnowPeer(const uint8_t*);
        ~EspnowPeer();
        bool IsCorrectAddress(const uint8*);
        void ReceiveMessage(EspnowMessageRequest     message);
        void ReceiveMessage(EspnowMessageCalculation message);
        void ReceiveMessage(EspnowMessageKeepAlive  message);
        void ReceiveMessage(EspnowMessageAcknowledge message);
        bool IsAlive();
        void Refresh();
        void SendSubscriptionRequest();

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR
        const char* Log();
#endif
};

#endif