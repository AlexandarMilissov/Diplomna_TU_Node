#ifndef ESPNOWDRIVER_HPP_
#define ESPNOWDRIVER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "esp_now.h"
#include <vector>
#include <queue>
#include "BaseComponent.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"
#include "LogManager.hpp"
#include "MacAddress.hpp"
#include "TaskManager.hpp"

typedef std::tuple<MacAddress, RSSI_Type , int, uint8*> ReceivedMessageTuple;

/**
 * @brief The EspnowDriver class provides functionality for initializing and sending data using ESP-NOW protocol.
 */
class EspnowDriver : public BaseComponent, public IMessageSender
{
private:
    static MacAddress myEspnowMac;
    static MacAddress broadcastEspnowMac;
    static MacAddress communicationChannelEspnowMac;

    static Spinlock sendLock;
    static std::vector<EspnowDriver*> drivers;

    static void InterruptReceive(const esp_now_recv_info_t*, const uint8*, int);
    void CyclicReceive();

    static uint16 tuplePoolSize;
    static std::queue<ReceivedMessageTuple*> tuplePool;
    static std::queue<ReceivedMessageTuple*> receivedMessagesQueue;
    static Spinlock receivedMessagesQueueSpinlock;

    std::vector<IMessageReceiver*> upperLayer;
    LogManager& logManager;
    TaskManager& taskManager;

    void Receive(const MacAddress, const RSSI_Type, const uint8*, int);
public:
    EspnowDriver(LogManager&, TaskManager&);
    ~EspnowDriver();

    void Init();

    void Subscribe(IMessageReceiver&);

    void Send(const NetIdentifier, const std::stack<Payload>);
    void SendBroadcast(const std::stack<Payload>);
};

#endif // ESPNOWDRIVER_HPP_
