#ifndef RSSI_MESSAGE_INTERFACE_HPP_
#define RSSI_MESSAGE_INTERFACE_HPP_

#include "Payload.hpp"

typedef enum EspnowMessageType
{
    NOW_REQUEST,
    NOW_CALCULATION,
    NOW_KEEP_ALIVE,
    NOW_ACKNOWLEDGE
}EspnowMessageType;
#define MessageTypeSize 1

class IEspnowMessage
{
    public:
        virtual Payload GetPayload() const = 0;
        virtual ~IEspnowMessage() = default;
};

#endif // RSSI_MESSAGE_INTERFACE_HPP_
