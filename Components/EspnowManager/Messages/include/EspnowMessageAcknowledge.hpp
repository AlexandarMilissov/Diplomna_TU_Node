#ifndef RSSI_MESSAGE_ACKNOWLEDGE_HPP_
#define RSSI_MESSAGE_ACKNOWLEDGE_HPP_

#include "IEspnowMessage.hpp"
#include "Payload.hpp"

class EspnowMessageAcknowledge: public IEspnowMessage
{
private:
    bool status;
    static uint8 GetElementsSize();
public:
    EspnowMessageAcknowledge(Payload);
    EspnowMessageAcknowledge(bool);
    bool GetStatus();
    Payload GetPayload() const;
};

#endif // RSSI_MESSAGE_ACKNOWLEDGE_HPP_