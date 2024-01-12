#ifndef RSSI_MESSAGE_CALCULATION_HPP_
#define RSSI_MESSAGE_CALCULATION_HPP_

#include "OpenSeries.hpp"
#include "IEspnowMessage.hpp"

class EspnowMessageCalculation: public IEspnowMessage
{
private:
    RSSI_Type RSSI;
    SeriesId seriesId;
    MessagePositionId messagePositionId;
    static SeriesId send_series_Id;
    static MessagePositionId send_message_Position_Id;
public:
    EspnowMessageCalculation(std::queue<Payload>);
    EspnowMessageCalculation();
    SeriesId GetSeriesID();
    MessagePositionId GetMessagePosition();
    RSSI_Type GetRSSI();

    std::stack<Payload> GetPayload() const;
};


#endif // RSSI_MESSAGE_CALCULATION_HPP_