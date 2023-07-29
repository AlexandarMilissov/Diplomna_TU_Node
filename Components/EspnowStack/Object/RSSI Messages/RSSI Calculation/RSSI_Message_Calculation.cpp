#include "RSSI_Message_Calculation.hpp"
#include "OpenSeries.hpp"

uint8 RSSI_Message_Calculation::GetElementsSize()
{
    return sizeof(series_Id) + sizeof(message_Position_Id);
}

// On received
RSSI_Message_Calculation::RSSI_Message_Calculation(RSSI_Type rssi, MessageStruct messageStruct) : RSSI(rssi)
{
    if(messageStruct.messageSize != GetElementsSize())
    {
        throw ValidationFailedException();
    }
    series_Id = *((Series_Id*)messageStruct.message);
    message_Position_Id = *((Message_Position_Id*)((Series_Id*)messageStruct.message + sizeof(Series_Id)));
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

void RSSI_Message_Calculation::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);
    *((Series_Id*)data) = 0;
    *((Message_Position_Id*)data + sizeof(Series_Id)) = 0;
    
    MessageSend(RSSI_CALCULATION, data_size, data);

    free(data);
}

void RSSI_Message_Calculation::StaticSend(void* args)
{
    printf("RSSI_Message_Calculation::StaticSend task main function\n");
    //MessageSend(RSSI_CALCULATION, *((Series_Id*)args), NULL);
}
