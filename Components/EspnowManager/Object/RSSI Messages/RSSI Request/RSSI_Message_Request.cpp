#include "RSSI_Message_Request.hpp"

uint8 RSSI_Message_Request::GetElementsSize()
{
    return sizeof(info);
}

RSSI_Message_Request::RSSI_Message_Request(MessageStruct messageStruct)
{
    if(messageStruct.messageSize != GetElementsSize())
    {
        throw ValidationFailedException();
    }
    info = *((uint8*)messageStruct.message);
}

RSSI_Message_Request::RSSI_Message_Request(uint16 info)
{

}

RSSI_Message_Request::~RSSI_Message_Request()
{
}

void RSSI_Message_Request::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);

    *((uint16*)data) = info;
    
    MessageSend(RSSI_REQUEST, data_size, data);

    free(data);
}
