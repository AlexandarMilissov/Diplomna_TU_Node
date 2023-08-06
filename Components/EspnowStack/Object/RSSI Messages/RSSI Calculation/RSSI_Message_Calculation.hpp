#ifndef RSSI_MESSAGE_CALCULATION_HPP_
#define RSSI_MESSAGE_CALCULATION_HPP_

#include "SeriesCommon.hpp"
#include "Espnow_Message_General.h"

class RSSI_Message_Calculation
{
private:
    RSSI_Type RSSI;
    Series_Id series_Id;
    Message_Position_Id message_Position_Id;
    static uint8 GetElementsSize();
public:
    RSSI_Message_Calculation(RSSI_Type, Message);
    RSSI_Message_Calculation();
    ~RSSI_Message_Calculation();
    Series_Id GetSeriesID();
    Message_Position_Id GetMessagePosition();
    RSSI_Type GetRSSI();
    void Send(uint8*);
    static void StaticSend();
};


#endif // RSSI_MESSAGE_CALCULATION_HPP_