#ifndef IMESSAGESENDER_HPP_
#define IMESSAGESENDER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "MacAddress.hpp"
#include <stack>

class IMessageSender
{
public:
    virtual void Send(const MacAddress, const std::stack<Payload>) = 0;
    virtual void SendBroadcast(const std::stack<Payload>) = 0;
    virtual ~IMessageSender() = default;
};

#endif // IMESSAGESENDER_HPP_