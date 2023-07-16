#ifndef RSSI_MESSAGE_CALCULATION_HPP_
#define RSSI_MESSAGE_CALCULATION_HPP_

#include "RSSI_Message_Interface.hpp"
#include "Espnow_Message_General.h"

class RSSI_Message_Calculation : public RSSI_Message_Interface
{
private:
    RSSI_Type RSSI;
    uint8 GetElementsSize();
public:
    RSSI_Message_Calculation(RSSI_Type);
    RSSI_Message_Calculation();
    ~RSSI_Message_Calculation();
    RSSI_Type GetRSSI();
    void Send();
};


#endif // RSSI_MESSAGE_CALCULATION_HPP_