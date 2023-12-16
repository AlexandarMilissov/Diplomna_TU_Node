#ifndef RSSI_MESSAGE_KEEP_ALIVE_HPP_
#define RSSI_MESSAGE_KEEP_ALIVE_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "IEspnowMessage.hpp"

class EspnowMessageKeepAlive: public IEspnowMessage
{
private:
    static uint8 GetElementsSize();
public:
    EspnowMessageKeepAlive(Payload);
    EspnowMessageKeepAlive();

    Payload GetPayload() const;
};

#endif // RSSI_MESSAGE_KEEP_ALIVE_HPP_