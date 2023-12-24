#ifndef ESPNOWDRIVER_HPP_
#define ESPNOWDRIVER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "esp_now.h"
#include <vector>
#include "IComponent.hpp"
#include "IMessageable.hpp"
#include "IDriver.hpp"
#include "LogManager.hpp"

/**
 * @brief The EspnowDriver class provides functionality for initializing and sending data using ESP-NOW protocol.
 */
class EspnowDriver : public IComponent, public IDriver
{
private:
    /**< The spinlock used for thread-safe sending of data. */
    static Spinlock sendLock;
    /**< The vector that stores the instances of all EspnowDrivers. */
    static std::vector<EspnowDriver*> drivers;
    /**< The internal function for receiving data. */
    static void InternalReceive(const esp_now_recv_info_t*, const uint8*, int);
    /**< The vector that stores the callbacks for the received functions. */
    std::vector<IMessageable*> upperLayerMessageables;

    LogManager& logManager;
public:
    /**
     * @brief This class represents the EspnowDriver.
     *
     * The EspnowDriver class is responsible for low level handling communication
     * using the ESP-NOW protocol.
     */
    EspnowDriver(LogManager&);
    ~EspnowDriver();

    /**
     * @brief Initializes the EspnowDriver.
     *
     * This function initializes the EspnowDriver with the provided data.
     *
     * @param data A pointer to the data needed for initialization.
     */
    void Init();

    /**
     * @brief Subscribes a callback function to be called when an ESP-NOW message is received.
     *
     * @param callback The callback function to be subscribed.
     */

    void Subscribe(IMessageable&);
    /**
     * @brief Sends data using the ESP-NOW protocol.
     * @param data The data to be sent.
     * @param payload The payload associated with the data.
     */

    void Send(const Payload, const Payload);
    void Receive(const Payload*, const Payload*);
};

#endif // ESPNOWDRIVER_HPP_
