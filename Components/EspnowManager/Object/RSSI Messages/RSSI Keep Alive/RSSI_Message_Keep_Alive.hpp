#ifndef RSSI_MESSAGE_KEEP_ALIVE_HPP_
#define RSSI_MESSAGE_KEEP_ALIVE_HPP_

#include "RSSI_Message_Interface.hpp"
#include "Espnow_Message_General.h"

class RSSI_Message_Keep_Alive : public RSSI_Message_Interface
{
private:
    RSSI_Type RSSI;
    uint8 GetElementsSize();
public:
    RSSI_Message_Keep_Alive(RSSI_Type);
    ~RSSI_Message_Keep_Alive();
    static void StaticSend();
    void Send();
};

#endif // RSSI_MESSAGE_KEEP_ALIVE_HPP_