#include "RSSI_Message_Acknowledge.hpp"

uint8 RSSI_Message_Acknowledge::GetElementsSize()
{
    return sizeof(RSSI_Acknowledge_Status);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(MessageStruct messageStruct)
{
    if(messageStruct.messageSize != GetElementsSize())
    {
        throw ValidationFailedException();
    }
    status = *((RSSI_Acknowledge_Status*)messageStruct.message);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(RSSI_Acknowledge_Status _status)
{
    status = _status;
}

RSSI_Message_Acknowledge::~RSSI_Message_Acknowledge()
{
}

RSSI_Acknowledge_Status RSSI_Message_Acknowledge::GetStatus()
{
    return status;
}

void RSSI_Message_Acknowledge::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);

    *((RSSI_Acknowledge_Status*)data) = status;
    
    MessageSend(RSSI_ACKNOWLEDGE, data_size, data);

    free(data);
}
