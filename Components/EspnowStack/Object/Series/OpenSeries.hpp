#ifndef SERIES_HPP_
#define SERIES_HPP_

#include "Common.h"
#include "SeriesCommon.hpp"
#include "ClosedSeries.hpp"

class OpenSeries
{
    public:
        static const uint8 numberOfMessagesPerSeries = CONFIG_NUMBER_OF_MESSAGES_PER_SERIES;
    private:
        static const uint8 minimumNumberOfMessagesForCalculation = CONFIG_MINIMUM_NUMBER_OF_MESSAGES_PER_SERIES;
        RSSI_Type messagesRSSI[numberOfMessagesPerSeries] = {0};
        RSSI_Type totalRSSIOfMessages = 0;
        const Series_Id id;
        bool isClosed = false;
        uint8 counter = 0;
    public:
        OpenSeries(Series_Id);
        ~OpenSeries();
        void AddValue(Message_Position_Id, RSSI_Type);
        bool IsCorrectId(Series_Id);
        ClosedSeries* CloseSeries();
};

#endif // SERIES_HPP_