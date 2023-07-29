#ifndef RSSI_MESSAGE_REQUEST_HPP
#define RSSI_MESSAGE_REQUEST_HPP

#include "EspnowDriver_Upper.h"
#include "RSSI_Message_Interface.hpp"
#include "OpenSeries.hpp"

#define SUBSCRIBE true
#define UNSUBSCRIBE false

class RSSI_Message_Request : public RSSI_Message_Interface
{
private:
    bool subsricptionStatus = UNSUBSCRIBE;
    uint8 GetElementsSize();
public:
    RSSI_Message_Request(MessageStruct);
    RSSI_Message_Request(bool);
    ~RSSI_Message_Request();
    bool GetSubsricptionStatus();
    void Send();
};


#endif
