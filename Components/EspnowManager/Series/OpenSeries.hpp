#ifndef SERIES_HPP_
#define SERIES_HPP_

#include "Common.hpp"

typedef sint32 DistanceUnits;

/**
 * @typedef SeriesId
 * @brief Represents a unique identifier for the series that a given node is sending.
 */
typedef uint32 SeriesId;

/**
 * @typedef MessagePositionId
 * @brief Represents a unique identifier for the position of a message in a series.
 */
typedef uint8 MessagePositionId;

class OpenSeries
{
    private:
        static const uint8 numberOfMessagesPerSeries = CONFIG_NUMBER_OF_MESSAGES_PER_SERIES;
        static const uint8 minimumNumberOfMessagesForCalculation = CONFIG_MINIMUM_NUMBER_OF_MESSAGES_PER_SERIES;
        RSSI_Type messagesRSSI[numberOfMessagesPerSeries] = {0};
        RSSI_Type totalRSSIOfMessages = 0;
        const SeriesId id;
        bool isClosed = false;
        uint8 counter = 0;
    public:
        OpenSeries(SeriesId);
        ~OpenSeries();
        static uint8 GetNumberOfMessagesPerSeries();
        void AddValue(MessagePositionId, RSSI_Type);
        bool IsCorrectId(SeriesId);
        DistanceUnits CloseSeries();
};

#endif // SERIES_HPP_