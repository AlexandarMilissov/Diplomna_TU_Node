#include "OpenSeries.hpp"
#include "Distance.hpp"

OpenSeries::OpenSeries(Series_Id _id) : id(_id)
{
}

OpenSeries::~OpenSeries()
{
}

SeriesError_Type OpenSeries::AddValue(Message_Position_Id messageId, RSSI_Type rssi)
{
#if CONFIG_USE_RSSI != FALSE
    DistanceUnits value = rssi;
#else
    DistanceUnits value = Distance::RSSI_To_DistanceUnits(rssi);
#endif

    if(isClosed)
    {
        return SERIES_CLOSED;
    }
    if(counter >= numberOfMessagesPerSeries)
    {
        return SERIES_FULL;
    }
    if(messageId >= numberOfMessagesPerSeries)
    {
        return WRONG_MESSAGE_ID;
    }
    if(0 != messagesRSSI[messageId])
    {
        return WRONG_MESSAGE_ID;
    }

    messagesRSSI[messageId] = value;
    totalRSSIOfMessages += value;
    counter++;

    return NO_ERROR;
}

bool OpenSeries::IsCorrectId(Series_Id _id)
{
    return (_id == id);
}

ClosedSeries* OpenSeries::CloseSeries()
{
    isClosed = true;
    ClosedSeries* closedSeries;
    if(minimumNumberOfMessagesForCalculation >= counter)
    {
        closedSeries = new ClosedSeries(0);
    }
    else
    {
        closedSeries = new ClosedSeries(((float)totalRSSIOfMessages) / counter);
    }

    return closedSeries;
}

