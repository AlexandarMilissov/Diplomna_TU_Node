#ifndef RSSI_MESSAGE_REQUEST_HPP
#define RSSI_MESSAGE_REQUEST_HPP

#include "RSSI_Message_Interface.hpp"
#include "Espnow_Message_General.h"

class RSSI_Message_Request : public RSSI_Message_Interface
{
private:
    uint16 info;
    uint8 GetElementsSize();
public:
    RSSI_Message_Request(MessageStruct);
    RSSI_Message_Request(uint16);
    ~RSSI_Message_Request();
    void Send();
};


#endif
