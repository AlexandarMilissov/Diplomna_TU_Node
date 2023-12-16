#ifndef ESPNOWDRIVER_HPP_
#define ESPNOWDRIVER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "esp_now.h"
#include <vector>

typedef void (*EspnowCallback)(const uint8_t*, const Payload*, const RSSI_Type);

/**
 * @brief The EspnowDriver class provides functionality for initializing and sending data using ESP-NOW protocol.
 */
class EspnowDriver
{
private:
    /**< The spinlock used for thread-safe sending of data. */
    static Spinlock sendLock;
    /**< The vector that stores the callbacks for the received functions. */
    static std::vector<EspnowCallback> ul_callbacks;
    /**< The internal function for receiving data. */
    static void Receive(const esp_now_recv_info_t*, const uint8*, int);
public:
    /**
     * @brief Initializes the EspnowDriver.
     *
     * This function initializes the EspnowDriver with the provided data.
     *
     * @param data A pointer to the data needed for initialization.
     */
    static void Init(const void*);

    /**
     * @brief Subscribes a callback function to be called when an ESP-NOW message is received.
     *
     * @param callback The callback function to be subscribed.
     */
    static void Subscribe(EspnowCallback callback);

    /**
     * @brief Sends data using the ESP-NOW protocol.
     * @param data The data to be sent.
     * @param payload The payload associated with the data.
     */
    static void Send(const uint8*, const Payload*);
};

#endif // ESPNOWDRIVER_HPP_
