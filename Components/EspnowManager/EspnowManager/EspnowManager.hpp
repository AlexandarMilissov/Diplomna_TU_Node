#ifndef ESPNOWMANAGER_HPP_
#define ESPNOWMANAGER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "EspnowDriver.hpp"
#include <stdatomic.h>
#include <queue>
#include <vector>
#include <tuple>

#include "BaseComponent.hpp"
#include "IEspnowController.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "IMonitorable.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"

#include "Monitor.hpp"

class EspnowPeer;

typedef enum
{
    NOW_NO_INIT,    // Before any values are initialized
    NOW_INIT,       // After everything has been initialized, Network is not active
    NOW_RUN,        // Network is active
}EspnowManagerState;

class EspnowManager : public BaseComponent, public IEspnowController, public IMessageReceiver, public IMonitorable
{
private:
    std::atomic<EspnowManagerState> internalState = NOW_NO_INIT;
    std::atomic<uint16> calculationSubscribers = 0;

    std::vector<EspnowPeer*> espnowPeers;
    Spinlock peerListLock = Spinlock_Init;

    LogManager& logManager;
    IScheduler& taskManager;
    IMessageSender& lowerLayer;

    void MainFunctionUpdatePeers();
    void MainFunctionSendCyclicKeepAlive();
    void MainFunctionSeriesBegin();
    void SendCalculationSeries();

public:
    EspnowManager(
        LogManager& logManager,
        IScheduler& taskManager,
        IMessageSender& lowerLayer
    );

    ~EspnowManager();

    void ActivateNetwork();
    void DeactivateNetwork();
    void Subscribe();
    void Unsubscribe();

    void Init();

    void Receive(const NetIdentifier, const std::queue<Payload>);

    std::string GetMonitorData();
};


#endif // ESPNOWMANAGER_HPP_