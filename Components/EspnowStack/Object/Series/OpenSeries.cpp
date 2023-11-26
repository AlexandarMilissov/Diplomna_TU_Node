#include "OpenSeries.hpp"
#include "Distance.hpp"

OpenSeries::OpenSeries(Series_Id _id) : id(_id)
{
}

OpenSeries::~OpenSeries()
{
}

/**
 * @brief Add a value to the OpenSeries instance based on a message ID and RSSI.
 *
 * This function adds a distance value to the OpenSeries instance based on the provided
 * message ID and RSSI (Received Signal Strength Indication). The RSSI value is either used
 * directly or converted to a distance value, depending on the configuration.
 *
 * @param messageId A unique identifier for the message position.
 * @param rssi The Received Signal Strength Indication (RSSI) associated with the message.
 */
void OpenSeries::AddValue(Message_Position_Id messageId, RSSI_Type rssi)
{
    // Check if the OpenSeries instance is already closed.
    if(isClosed)
    {
        return;
    }
    // Check if the counter has reached the maximum number of messages per series.
    if(counter >= numberOfMessagesPerSeries)
    {
        return;
    }
    // Check if the provided message ID is within the valid range.
    if(messageId >= numberOfMessagesPerSeries)
    {
        return;
    }
    // Check if the message ID has not been used already.
    if(0 != messagesRSSI[messageId])
    {
        return;
    }

    // Add the value to the messages array and update relevant counters.
    messagesRSSI[messageId] = rssi;
    totalRSSIOfMessages += rssi;
    counter++;
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
        closedSeries = new ClosedSeries();
    }
    else
    {
        float average = ((float)totalRSSIOfMessages) / counter;
        DistanceUnits value = Distance::Float_To_DistanceUnits(average);

        closedSeries = new ClosedSeries(value);
    }

    return closedSeries;
}

