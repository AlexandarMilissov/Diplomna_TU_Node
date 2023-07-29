#include "RSSI_Message_Request.hpp"

// Functions for receiving messages
RSSI_Message_Request::RSSI_Message_Request(MessageStruct messageStruct)
{
    if(messageStruct.messageSize != GetElementsSize())
    {
        throw ValidationFailedException();
    }
    subsricptionStatus = *((uint8*)messageStruct.message);
}

// Functions for sending messages
RSSI_Message_Request::RSSI_Message_Request(bool value)
{
    subsricptionStatus = value;
}

void RSSI_Message_Request::Send()
{
    size_t data_size = GetElementsSize();
    void* data = malloc(data_size);

    *((bool*)data) = subsricptionStatus;
    
    MessageSend(RSSI_REQUEST, data_size, data);

    free(data);
}

// Other functions

uint8 RSSI_Message_Request::GetElementsSize()
{
    return sizeof(subsricptionStatus);
}

RSSI_Message_Request::~RSSI_Message_Request()
{
}

bool RSSI_Message_Request::GetSubsricptionStatus()
{
    return subsricptionStatus;
}
