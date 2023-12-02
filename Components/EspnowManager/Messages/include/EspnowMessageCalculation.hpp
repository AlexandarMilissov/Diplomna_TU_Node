#ifndef RSSI_MESSAGE_CALCULATION_HPP_
#define RSSI_MESSAGE_CALCULATION_HPP_

#include "SeriesCommon.hpp"
#include "IEspnowMessage.hpp"
#include "Payload.hpp"

class EspnowMessageCalculation: public IEspnowMessage
{
private:
    RSSI_Type RSSI;
    Series_Id series_Id;
    Message_Position_Id message_Position_Id;
    static uint8 GetElementsSize();
    static Series_Id send_series_Id;
    static Message_Position_Id send_message_Position_Id;
public:
    EspnowMessageCalculation(RSSI_Type, Payload);
    EspnowMessageCalculation();
    Series_Id GetSeriesID();
    Message_Position_Id GetMessagePosition();
    RSSI_Type GetRSSI();

    Payload GetPayload() const;
};


#endif // RSSI_MESSAGE_CALCULATION_HPP_