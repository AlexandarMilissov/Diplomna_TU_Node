#ifndef ESPNOWMANAGER_HPP_
#define ESPNOWMANAGER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "EspnowPeer.hpp"
#include "EspnowDriver.hpp"
#include <stdatomic.h>
#include <queue>
#include <vector>

#include "IComponent.hpp"
#include "IMessageable.hpp"
#include "IDriver.hpp"
#include "IMonitorable.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"

#include "Monitor.hpp"

typedef enum
{
    NO_INIT,    // Before any values are initialized
    INIT,       // After everything has been initialized, Network is not active
    RUN,        // Network is active
}InternalState;

// TODO: Update this
typedef struct InterruptReceivedMessageStruct
{
    Payload* src_addr;
    Payload* message;
}InterruptReceivedMessageStruct;

class EspnowManager : public IComponent, public IEspnowController, public IMessageable, public IMonitorable
{
private:
    std::atomic<InternalState> internalState = NO_INIT;
    std::atomic<uint16> calculationSubscribers = 0;

    uint64 handledMessagesCounter  = 0;
    uint64 receivedMessagesCounter = 0;
    std::queue<InterruptReceivedMessageStruct*> interruptReceivedMessages;
    Spinlock InterruptReceivedMessagesSpinlock = Spinlock_Init;

    std::vector<EspnowPeer*> Peers;
    Spinlock peerListLock = Spinlock_Init;
    void HandleReceivedMessages();
    void HandleReceivedMessage(const InterruptReceivedMessageStruct*);

    IDriver& driver;
    LogManager& logManager;
    IScheduler& taskManager;

    void MainFunctionUpdatePeers();
    void MainFunctionSendCyclicKeepAlive();
    void MainFunctionSeriesBegin();
    void MainFunctionHandleReceivedMessages();
    void SendCalculationSeries();

public:
    EspnowManager(
        IDriver& driver,
        LogManager& logManager,
        IScheduler& taskManager
        ) :
        driver(driver),
        logManager(logManager),
        taskManager(taskManager)
        {}

    void ActivateNetwork();
    void DeactivateNetwork();
    void Subscribe();
    void Unsubscribe();

    void Init();

    void Send(const Payload, const Payload);
    void Receive(const Payload*, const Payload*);

    std::string GetMonitorData();
};


#endif // ESPNOWMANAGER_HPP_