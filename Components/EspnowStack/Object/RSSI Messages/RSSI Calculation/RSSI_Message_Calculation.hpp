#ifndef RSSI_MESSAGE_CALCULATION_HPP_
#define RSSI_MESSAGE_CALCULATION_HPP_

#include "RSSI_Message_Interface.hpp"
#include "EspnowDriver_Upper.h"
#include "SeriesCommon.hpp"

class RSSI_Message_Calculation : public RSSI_Message_Interface
{
private:
    RSSI_Type RSSI;
    Series_Id series_Id;
    Message_Position_Id message_Position_Id;
    uint8 GetElementsSize();
public:
    RSSI_Message_Calculation(RSSI_Type, MessageStruct);
    RSSI_Message_Calculation();
    ~RSSI_Message_Calculation();
    Series_Id GetSeriesID();
    Message_Position_Id GetMessagePosition();
    RSSI_Type GetRSSI();
    void Send();
    static void StaticSend(void*);
};


#endif // RSSI_MESSAGE_CALCULATION_HPP_