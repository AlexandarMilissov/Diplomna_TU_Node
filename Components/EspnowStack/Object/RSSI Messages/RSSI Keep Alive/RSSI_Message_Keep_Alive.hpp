#ifndef RSSI_MESSAGE_KEEP_ALIVE_HPP_
#define RSSI_MESSAGE_KEEP_ALIVE_HPP_

#include "EspnowMessageGeneral.h"
#include "EspnowMessageInteface.hpp"

class RSSI_Message_Keep_Alive: EspnowMessageInteface
{
private:
    RSSI_Type RSSI;
    static uint8 GetElementsSize();
public:
    RSSI_Message_Keep_Alive(RSSI_Type);
    ~RSSI_Message_Keep_Alive();
    void Send(uint8*);
    static void StaticSend();
};

#endif // RSSI_MESSAGE_KEEP_ALIVE_HPP_