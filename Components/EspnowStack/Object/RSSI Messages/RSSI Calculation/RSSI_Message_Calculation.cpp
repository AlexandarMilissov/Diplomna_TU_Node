#include "RSSI_Message_Calculation.hpp"
#include "OpenSeries.hpp"
#include "EspnowManager_Communication.hpp"
#include <stdexcept>

static Series_Id send_series_Id = 0;
static Message_Position_Id send_message_Position_Id = 0;
struct RSSI_Message_Calculation_Struct
{
    Series_Id series_Id;
    Message_Position_Id message_Position_Id;
};

uint8 RSSI_Message_Calculation::GetElementsSize()
{
    return sizeof(series_Id) + sizeof(message_Position_Id);
}

// On received
RSSI_Message_Calculation::RSSI_Message_Calculation(RSSI_Type rssi, Message message) : RSSI(rssi)
{
    if(message.data_size != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    struct RSSI_Message_Calculation_Struct data_s = *((RSSI_Message_Calculation_Struct*)message.data);
    series_Id = data_s.series_Id;
    message_Position_Id = data_s.message_Position_Id;
}

RSSI_Message_Calculation::RSSI_Message_Calculation()
{
}

RSSI_Message_Calculation::~RSSI_Message_Calculation()
{
}

Series_Id RSSI_Message_Calculation::GetSeriesID()
{
    return series_Id;
}

Message_Position_Id RSSI_Message_Calculation::GetMessagePosition()
{
    return message_Position_Id;
}

RSSI_Type RSSI_Message_Calculation::GetRSSI()
{
    return RSSI;
}

void RSSI_Message_Calculation::Send(uint8* dst_addr)
{
    StaticSend();
}

void RSSI_Message_Calculation::StaticSend()
{
    Message* message = MessageInit(GetElementsSize());

    ((struct RSSI_Message_Calculation_Struct*)(message->data))->series_Id           = send_series_Id;
    ((struct RSSI_Message_Calculation_Struct*)(message->data))->message_Position_Id = send_message_Position_Id;

    SendMessage(broadcast_mac, RSSI_CALCULATION, message);

    if((OpenSeries::GetNumberOfMessagesPerSeries() - 1) > send_message_Position_Id)
    {
        send_message_Position_Id++;
    }
    else
    {
        send_series_Id++;
        send_message_Position_Id = 0;
    }

    MessageDeinit(message);
}
