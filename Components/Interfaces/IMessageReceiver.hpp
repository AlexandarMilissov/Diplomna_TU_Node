#ifndef IMESSAGEABLE_HPP_
#define IMESSAGEABLE_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "MacAddress.hpp"
#include <stack>

class IMessageReceiver
{
public:
    virtual void Receive(const MacAddress, std::queue<Payload>) = 0;
    virtual ~IMessageReceiver() = default;
};

#endif // IMESSAGEABLE_HPP_
