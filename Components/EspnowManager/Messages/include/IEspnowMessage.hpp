#ifndef RSSI_MESSAGE_INTERFACE_HPP_
#define RSSI_MESSAGE_INTERFACE_HPP_

#include "Payload.hpp"

class IEspnowMessage
{
    public:
        virtual std::stack<Payload> GetPayload() const = 0;
        virtual ~IEspnowMessage() = default;
};

#endif // RSSI_MESSAGE_INTERFACE_HPP_
