#include "RSSI_Message_Acknowledge.hpp"

uint8 RSSI_Message_Acknowledge::GetElementsSize()
{
    return sizeof(status);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(MessageStruct messageStruct)
{
    if(messageStruct.messageSize != GetElementsSize())
    {
        throw ValidationFailedException();
    }
    status = *((bool*)messageStruct.message);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(bool _status)
{
    status = _status;
}

RSSI_Message_Acknowledge::~RSSI_Message_Acknowledge()
{
}

bool RSSI_Message_Acknowledge::GetStatus()
{
    return status;
}

void RSSI_Message_Acknowledge::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);

    *((bool*)data) = status;
    MessageSend(RSSI_ACKNOWLEDGE, data_size, data);

    free(data);
}
