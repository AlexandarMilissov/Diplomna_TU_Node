#ifndef RSSI_MESSAGE_INTERFACE_HPP_
#define RSSI_MESSAGE_INTERFACE_HPP_

#include "Payload.hpp"

typedef enum MessageType
{
    RSSI_REQUEST,
    RSSI_CALCULATION,
    RSSI_KEEP_ALIVE,
    RSSI_ACKNOWLEDGE
}MessageType;
#define MessageTypeSize 1

class IEspnowMessage
{
    public:
        virtual Payload GetPayload() const = 0;
        virtual ~IEspnowMessage() = default;
};

#endif // RSSI_MESSAGE_INTERFACE_HPP_
