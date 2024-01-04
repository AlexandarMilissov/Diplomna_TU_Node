#ifndef IMESSAGESENDER_HPP_
#define IMESSAGESENDER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "MacAddress.hpp"

class IMessageSender
{
public:
    virtual void Send(const MacAddress, const Payload) = 0;
    virtual void SendBroadcast(const Payload) = 0;
    virtual ~IMessageSender() = default;
};

#endif // IMESSAGESENDER_HPP_