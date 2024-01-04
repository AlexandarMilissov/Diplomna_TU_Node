#ifndef RSSI_MESSAGE_KEEP_ALIVE_HPP_
#define RSSI_MESSAGE_KEEP_ALIVE_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "IEspnowMessage.hpp"

class EspnowMessageKeepAlive: public IEspnowMessage
{
public:
    EspnowMessageKeepAlive(std::queue<Payload>);
    EspnowMessageKeepAlive();

    std::stack<Payload> GetPayload() const;
};

#endif // RSSI_MESSAGE_KEEP_ALIVE_HPP_