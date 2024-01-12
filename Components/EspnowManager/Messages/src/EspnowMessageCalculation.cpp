#include "EspnowMessageCalculation.hpp"
#include "OpenSeries.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>
#include <string.h>

SeriesId EspnowMessageCalculation::send_series_Id = 0;
MessagePositionId EspnowMessageCalculation::send_message_Position_Id = 0;

EspnowMessageCalculation::EspnowMessageCalculation(std::queue<Payload> payloadQueueOriginal)
{
    // Copy the queue to not modify the original
    std::queue<Payload> payloadQueue = payloadQueueOriginal;

    {
        // Check if the queue is empty
        if(payloadQueue.empty())
        {
            throw std::invalid_argument(std::string("Payload queue is empty!\n"));
        }

        // Get the series id
        Payload seriesIdPayload = payloadQueue.front();
        payloadQueue.pop();
        seriesId = *((SeriesId*)seriesIdPayload.GetData());
    }
    {
        // Check if the queue is empty
        if(payloadQueue.empty())
        {
            throw std::invalid_argument(std::string("Payload queue is empty!\n"));
        }

        // Get the message position id
        Payload messagePositionIdPayload = payloadQueue.front();
        payloadQueue.pop();
        messagePositionId = *((MessagePositionId*)messagePositionIdPayload.GetData());
    }
    {
        // Check if the queue is empty
        if(payloadQueue.empty())
        {
            throw std::invalid_argument(std::string("Payload queue is empty!\n"));
        }

        // Get the RSSI
        Payload rssiPayload = payloadQueue.front();
        payloadQueue.pop();
        RSSI = *((RSSI_Type*)rssiPayload.GetData());
    }
}

EspnowMessageCalculation::EspnowMessageCalculation()
{
    seriesId = send_series_Id;
    messagePositionId = send_message_Position_Id;

    if((OpenSeries::GetNumberOfMessagesPerSeries() - 1) > send_message_Position_Id)
    {
        send_message_Position_Id++;
    }
    else
    {
        send_series_Id++;
        send_message_Position_Id = 0;
    }
}

SeriesId EspnowMessageCalculation::GetSeriesID()
{
    return seriesId;
}

MessagePositionId EspnowMessageCalculation::GetMessagePosition()
{
    return messagePositionId;
}

RSSI_Type EspnowMessageCalculation::GetRSSI()
{
    return RSSI;
}

std::stack<Payload> EspnowMessageCalculation::GetPayload() const
{
    EspnowMessageType messageType = NOW_CALCULATION;

    Payload seriesIdPayload((uint8*)(&seriesId), sizeof(seriesId));
    Payload messagePositionIdPayload((uint8*)(&messagePositionId), sizeof(messagePositionId));
    Payload messageTypePayload((uint8*)(&messageType), sizeof(messageType));

    std::stack<Payload> payloadQueue;
    payloadQueue.push(messagePositionIdPayload);
    payloadQueue.push(seriesIdPayload);
    payloadQueue.push(messageTypePayload);

    return payloadQueue;
}