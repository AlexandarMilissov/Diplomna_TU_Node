#ifndef ESPNOWDRIVER_HPP_
#define ESPNOWDRIVER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "esp_now.h"
#include <vector>
#include <queue>
#include "IComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "MacAddress.hpp"
#include "TaskManager.hpp"

typedef std::tuple<esp_now_recv_info_t*, uint8*, int> ReceivedMessageTuple;

/**
 * @brief The EspnowDriver class provides functionality for initializing and sending data using ESP-NOW protocol.
 */
class EspnowDriver : public IComponent, public IMessageSender, public IMessageReceiver
{
private:
    static MacAddress myEspnowMac;
    static MacAddress broadcastEspnowMac;
    static MacAddress communicationChannelEspnowMac;

    static Spinlock sendLock;
    static std::vector<EspnowDriver*> drivers;

    static void InterruptReceive(const esp_now_recv_info_t*, const uint8*, int);
    static void CyclicReceive();

    static uint16 tuplePoolSize;
    static std::queue<ReceivedMessageTuple*> tuplePool;
    static std::queue<ReceivedMessageTuple*> receivedMessagesQueue;
    static Spinlock receivedMessagesQueueSpinlock;

    std::vector<IMessageReceiver*> upperLayerMessageables;
    LogManager& logManager;
    TaskManager& taskManager;
public:
    EspnowDriver(LogManager&, TaskManager&);
    ~EspnowDriver();

    void Init();

    void Subscribe(IMessageReceiver&);

    void Send(const MacAddress, const Payload);
    void SendBroadcast(const Payload);
    void Receive(const MacAddress, const Payload);
};

#endif // ESPNOWDRIVER_HPP_
