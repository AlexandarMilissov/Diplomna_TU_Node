#ifndef RSSI_MESSAGE_REQUEST_HPP
#define RSSI_MESSAGE_REQUEST_HPP

#include "OpenSeries.hpp"
#include "EspnowMessageGeneral.h"
#include "Payload.hpp"
#include "EspnowMessageInteface.hpp"

#define SUBSCRIBE true
#define UNSUBSCRIBE false

class RSSI_Message_Request : EspnowMessageInteface
{
private:
    bool subscriptionStatus = UNSUBSCRIBE;
    static uint8 GetElementsSize();
public:
    RSSI_Message_Request(Payload);
    RSSI_Message_Request(bool);
    ~RSSI_Message_Request();
    bool GetSubscriptionStatus();
    void Send(uint8*);
};


#endif
