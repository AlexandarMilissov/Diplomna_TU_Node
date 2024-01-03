#ifndef IMESSAGEABLE_HPP_
#define IMESSAGEABLE_HPP_

#include "Common.hpp"
#include "Payload.hpp"

class IMessageable
{
public:
    virtual void Send(const Payload, const Payload) = 0;
    virtual void SendBroadcast(const Payload) = 0;
    virtual void Receive(const Payload*, const Payload*) = 0;
    virtual ~IMessageable() = default;
};

#endif // IMESSAGEABLE_HPP_
