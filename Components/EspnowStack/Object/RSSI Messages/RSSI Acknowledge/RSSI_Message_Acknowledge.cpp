#include "RSSI_Message_Acknowledge.hpp"
#include "To_C_Encapsulation.h"
#include <stdexcept>

uint8 RSSI_Message_Acknowledge::GetElementsSize()
{
    return sizeof(status);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(Message* message)
{
    if(message->data_size != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    status = (bool)message->data;
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

void RSSI_Message_Acknowledge::Send(uint8* dst_addr)
{
    Message* message = MessageInit(GetElementsSize());

    *(message->data) = status;

    MessageSend(dst_addr, RSSI_ACKNOWLEDGE, message);
}
