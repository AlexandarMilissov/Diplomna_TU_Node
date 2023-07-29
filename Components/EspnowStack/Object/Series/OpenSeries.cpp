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
    DistanceUnits value = Distance::RSSI_To_DistanceUnits(rssi);
    
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

uint8 OpenSeries::GetNumberOfMessagesPerSeries()
{
    return numberOfMessagesPerSeries;
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

