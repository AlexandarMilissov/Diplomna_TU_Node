#ifndef ESPNOWMANAGER_INTERNAL_HPP_
#define ESPNOWMANAGER_INTERNAL_HPP_

#include <atomic>
#include <queue>
#include "Peer.hpp"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "Common.h"

typedef enum
{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}EspnowManagerInternalState;

typedef struct InterruptReceivedMessageStruct
{
    uint8 src_addr[6];
    Message* message;
    sint16 rx_ctrl_rssi;
}InterruptReceivedMessageStruct;

extern std::atomic<EspnowManagerInternalState> espnowManagerInternalState;
extern std::atomic<uint16> calculationSubscribers;
extern uint64 handledMessagesCounter;
extern uint64 receivedMessagesCounter;

extern std::queue<InterruptReceivedMessageStruct*> interruptReceivedMessages;
extern Spinlock InterruptReceivedMessagesSpinlock;

extern std::vector<Peer*> Peers;
extern Spinlock peerListProtection;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ESPNOWMANAGER_INTERNAL_HPP_