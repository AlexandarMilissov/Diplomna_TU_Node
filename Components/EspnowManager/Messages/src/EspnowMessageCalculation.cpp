#include "EspnowMessageCalculation.hpp"
#include "OpenSeries.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>
#include <string.h>

Series_Id EspnowMessageCalculation::send_series_Id = 0;
Message_Position_Id EspnowMessageCalculation::send_message_Position_Id = 0;
struct RSSI_Message_Calculation_Struct
{
    Series_Id series_Id;
    Message_Position_Id message_Position_Id;
};

uint8 EspnowMessageCalculation::GetElementsSize()
{
    return sizeof(series_Id) + sizeof(message_Position_Id);
}

// On received
EspnowMessageCalculation::EspnowMessageCalculation(RSSI_Type rssi, Payload message) : RSSI(rssi)
{
    if(message.GetSize() != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    struct RSSI_Message_Calculation_Struct data_s;
    memcpy(&data_s, message.data, GetElementsSize());
    series_Id = data_s.series_Id;
    message_Position_Id = data_s.message_Position_Id;
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

Payload EspnowMessageCalculation::GetPayload() const
{
    Payload data(GetElementsSize());

    struct RSSI_Message_Calculation_Struct dataStruct = {
        .series_Id = send_series_Id,
        .message_Position_Id = send_message_Position_Id,
    };

    memcpy(data.data, &dataStruct, GetElementsSize());

    Payload message(MessageTypeSize);
    *(message.data) = RSSI_CALCULATION;

    message += data;
    return message;
}