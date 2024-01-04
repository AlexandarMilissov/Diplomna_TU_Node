#ifndef ESPNOWMANAGER_HPP_
#define ESPNOWMANAGER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "EspnowDriver.hpp"
#include <stdatomic.h>
#include <queue>
#include <vector>
#include <tuple>

#include "IComponent.hpp"
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

class EspnowManager : public IComponent, public IEspnowController, public IMessageSender, public IMessageReceiver, public IMonitorable
{
private:
    std::atomic<EspnowManagerState> internalState = NOW_NO_INIT;
    std::atomic<uint16> calculationSubscribers = 0;

    std::vector<EspnowPeer*> Peers;
    Spinlock peerListLock = Spinlock_Init;

    IMessageSender& lowerLayer;
    LogManager& logManager;
    IScheduler& taskManager;

    void MainFunctionUpdatePeers();
    void MainFunctionSendCyclicKeepAlive();
    void MainFunctionSeriesBegin();
    void SendCalculationSeries();

public:
    EspnowManager(
        IMessageSender& lowerLayer,
        LogManager& logManager,
        IScheduler& taskManager
    );

    ~EspnowManager();

    void ActivateNetwork();
    void DeactivateNetwork();
    void Subscribe();
    void Unsubscribe();

    void Init();

    void Send(const MacAddress, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
    void Receive(const MacAddress, const std::queue<Payload>);

    std::string GetMonitorData();
};


#endif // ESPNOWMANAGER_HPP_