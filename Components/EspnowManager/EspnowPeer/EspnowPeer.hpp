#ifndef PEER_H
#define PEER_H

#include "Common.hpp"
#include "Distance.hpp"
#include "EspnowMessageRequest.hpp"
#include "EspnowMessageCalculation.hpp"
#include "EspnowMessageKeepAlive.hpp"
#include "EspnowMessageAcknowledge.hpp"
#include "IMessageSender.hpp"
#include "IEspnowController.hpp"
#include "IMonitorable.hpp"
#include "LogManager.hpp"
#include <atomic>
#include <queue>
#include <tuple>

class EspnowPeer : public IMonitorable
{
    private:
        static const uint8 seriesBeginningLife = CONFIG_LIFE_OF_SERIES;
        static const uint8 peerBeginningLife = CONFIG_LIFE_OF_PEERS;
        std::atomic<uint8> peerLife;
        IMessageSender& lowerLayer;
        IEspnowController& espnowController;
        LogManager logManager;
        Distance distance;
        MacAddress sourceAddress;
        std::vector<std::tuple<OpenSeries*, uint8>*> openSeries;
        uint16 failedSeries = 0;
        Spinlock subscriptionStateLock = Spinlock_Init;
        Spinlock calculationDataLock = Spinlock_Init;
        std::atomic<bool> isPeerSubscribedToUs   = false;
        std::atomic<bool> areWeSubscribedToPeer  = false;
        std::atomic<bool> acknowledgeRequired    = false;
        void ReceiveMessage(EspnowMessageRequest     message);
        void ReceiveMessage(EspnowMessageCalculation message);
        void ReceiveMessage(EspnowMessageKeepAlive   message);
        void ReceiveMessage(EspnowMessageAcknowledge message);
    public:
        EspnowPeer(IMessageSender&, IEspnowController&, LogManager&, const MacAddress);
        ~EspnowPeer();
        bool IsCorrectAddress(const MacAddress);
        bool IsAlive();
        void Refresh();
        void SendSubscriptionRequest();

        void Receive(const std::queue<Payload>);

        std::string GetMonitorData();
};

#endif