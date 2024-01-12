#include "EspnowMessageCalculation.hpp"
#include "OpenSeries.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>
#include <string.h>

Series_Id EspnowMessageCalculation::send_series_Id = 0;
Message_Position_Id EspnowMessageCalculation::send_message_Position_Id = 0;

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
        series_Id = *(seriesIdPayload.data);
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
        message_Position_Id = *(messagePositionIdPayload.data);
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
        RSSI = *((RSSI_Type*)rssiPayload.data);
    }
}

EspnowMessageCalculation::EspnowMessageCalculation()
{
    series_Id = send_series_Id;
    message_Position_Id = send_message_Position_Id;

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

Series_Id EspnowMessageCalculation::GetSeriesID()
{
    return series_Id;
}

Message_Position_Id EspnowMessageCalculation::GetMessagePosition()
{
    return message_Position_Id;
}

RSSI_Type EspnowMessageCalculation::GetRSSI()
{
    return RSSI;
}

std::stack<Payload> EspnowMessageCalculation::GetPayload() const
{
    EspnowMessageType messageType = NOW_CALCULATION;

    Payload seriesIdPayload((uint8*)(&series_Id), sizeof(series_Id));
    Payload messagePositionIdPayload((uint8*)(&message_Position_Id), sizeof(message_Position_Id));
    Payload messageTypePayload((uint8*)(&messageType), sizeof(messageType));

    std::stack<Payload> payloadQueue;
    payloadQueue.push(messagePositionIdPayload);
    payloadQueue.push(seriesIdPayload);
    payloadQueue.push(messageTypePayload);

    return payloadQueue;
}