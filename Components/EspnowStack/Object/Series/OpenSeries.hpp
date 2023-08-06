#ifndef SERIES_HPP_
#define SERIES_HPP_

#include "Common.h"
#include "SeriesCommon.hpp"
#include "ClosedSeries.hpp"

class OpenSeries
{
    public:
        static const uint8 numberOfMessagesPerSeries = 10;
    private:
        static const uint8 minimumNumberOfMessagesForCalculation = 8;
        DistanceUnits messagesRSSI[numberOfMessagesPerSeries] = {0};
        DistanceUnits totalRSSIOfMessages = 0;
        const Series_Id id;
        bool isClosed = false;
        uint8 counter = 0;
    public:
        OpenSeries(Series_Id);
        ~OpenSeries();
        SeriesError_Type AddValue(Message_Position_Id, RSSI_Type);
        bool IsCorrectId(Series_Id);
        ClosedSeries* CloseSeries();
};

#endif // SERIES_HPP_