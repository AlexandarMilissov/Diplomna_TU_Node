#ifndef RSSI_MESSAGE_ACKNOWLEDGE_HPP_
#define RSSI_MESSAGE_ACKNOWLEDGE_HPP_

#include "RSSI_Message_Interface.hpp"
#include "Espnow_Message_General.h"

typedef enum RSSI_Acknowledge_Status{
    SENDEND,
    REJECT
}RSSI_Acknowledge_Status;

class RSSI_Message_Acknowledge: public RSSI_Message_Interface
{
private:
    RSSI_Acknowledge_Status status;
    uint8 GetElementsSize();
public:
    RSSI_Message_Acknowledge(MessageStruct);
    RSSI_Message_Acknowledge(RSSI_Acknowledge_Status);
    ~RSSI_Message_Acknowledge();
    RSSI_Acknowledge_Status GetStatus();
    void Send();
};

#endif // RSSI_MESSAGE_ACKNOWLEDGE_HPP_