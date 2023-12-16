#ifndef ESPNOWMANAGER_HPP_
#define ESPNOWMANAGER_HPP_

#include "EspnowManager_Task_Config.h"

#include "Common.hpp"
#include "Payload.hpp"
#include "EspnowPeer.hpp"
#include <stdatomic.h>
#include <queue>
#include <vector>

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
#include "Monitor.hpp"
#endif

typedef enum
{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}EspnowManagerInternalState;

typedef struct InterruptReceivedMessageStruct
{
    uint8 src_addr[6];
    Payload* message;
    sint16 rx_ctrl_rssi;
}InterruptReceivedMessageStruct;

class EspnowManager
{
private:
    static std::atomic<EspnowManagerInternalState> espnowManagerInternalState;
    static std::atomic<uint16> calculationSubscribers;

    static uint64 handledMessagesCounter;
    static uint64 receivedMessagesCounter;
    static std::queue<InterruptReceivedMessageStruct*> interruptReceivedMessages;
    static Spinlock InterruptReceivedMessagesSpinlock;

    static std::vector<EspnowPeer*> Peers;
    static Spinlock peerListLock;
    static void HandleReceivedMessages();
    static void HandleReceivedMessage(const InterruptReceivedMessageStruct*);
    static void Receive(const uint8_t*, const Payload*, const RSSI_Type);
public:
    static void ActivateNetwork();
    static void DeactivateNetwork();
    static void Subscribe();
    static void Unsubscribe();

    static void Init(const void*);
    static void MainFunctionUpdatePeers(const void*);
    static void MainFunctionSendCyclicKeepAlive(const void*);
    static void MainFunctionSendCyclicCalculation(const void*);
    static void MainFunctionHandleReceivedMessages(const void*);
    static void SendCalculationSeries(const void*);

    static void Send(const uint8*, const Payload);

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
    static const char* Log();
#endif

};


#endif // ESPNOWMANAGER_HPP_