#ifndef RSSI_MESSAGE_REQUEST_HPP
#define RSSI_MESSAGE_REQUEST_HPP

#include "OpenSeries.hpp"
#include "Espnow_Message_General.h"

#define SUBSCRIBE true
#define UNSUBSCRIBE false

class RSSI_Message_Request
{
private:
    bool subsricptionStatus = UNSUBSCRIBE;
    static uint8 GetElementsSize();
public:
    RSSI_Message_Request(Message);
    RSSI_Message_Request(bool);
    ~RSSI_Message_Request();
    bool GetSubsricptionStatus();
    void Send(uint8*);
};


#endif
