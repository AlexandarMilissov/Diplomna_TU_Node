#ifndef RSSI_MESSAGE_ACKNOWLEDGE_HPP_
#define RSSI_MESSAGE_ACKNOWLEDGE_HPP_

#include "RSSI_Message_Interface.hpp"
#include "Espnow_Message_General.h"

class RSSI_Message_Acknowledge: public RSSI_Message_Interface
{
private:
    bool status;
    uint8 GetElementsSize();
public:
    RSSI_Message_Acknowledge(MessageStruct);
    RSSI_Message_Acknowledge(bool);
    ~RSSI_Message_Acknowledge();
    bool GetStatus();
    void Send();
};

#endif // RSSI_MESSAGE_ACKNOWLEDGE_HPP_