#include "RSSI_Message_Calculation.hpp"

uint8 RSSI_Message_Calculation::GetElementsSize()
{
    return 0;
}

RSSI_Message_Calculation::RSSI_Message_Calculation(RSSI_Type _RSSI)
{
    RSSI = _RSSI;
}

RSSI_Message_Calculation::RSSI_Message_Calculation()
{
}

RSSI_Message_Calculation::~RSSI_Message_Calculation()
{
}

RSSI_Type RSSI_Message_Calculation::GetRSSI()
{
    return RSSI;
}

void RSSI_Message_Calculation::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);
    
    MessageSend(RSSI_CALCULATION, data_size, data);

    free(data);
}
