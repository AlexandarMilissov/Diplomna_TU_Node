#include "RSSI_Message_Acknowledge.hpp"
#include "EspnowManager_Communication.hpp"
#include <stdexcept>

uint8 RSSI_Message_Acknowledge::GetElementsSize()
{
    return sizeof(status);
}

RSSI_Message_Acknowledge::RSSI_Message_Acknowledge(Payload* message)
{
    if(message->GetSize() != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    status = (bool)(message->data);
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
    Payload* message = new Payload(GetElementsSize());

    *(message->data) = status;

    SendMessage(dst_addr, RSSI_ACKNOWLEDGE, message);

    delete message;
}
