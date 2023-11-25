#ifndef RSSI_MESSAGE_ACKNOWLEDGE_HPP_
#define RSSI_MESSAGE_ACKNOWLEDGE_HPP_

#include "EspnowMessageGeneral.h"

class RSSI_Message_Acknowledge
{
private:
    bool status;
    static uint8 GetElementsSize();
public:
    RSSI_Message_Acknowledge(Message*);
    RSSI_Message_Acknowledge(bool);
    ~RSSI_Message_Acknowledge();
    bool GetStatus();
    void Send(uint8*);
};

#endif // RSSI_MESSAGE_ACKNOWLEDGE_HPP_