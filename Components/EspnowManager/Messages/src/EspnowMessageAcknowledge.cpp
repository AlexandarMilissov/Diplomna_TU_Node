#include "EspnowMessageAcknowledge.hpp"
#include "EspnowManager_Communication.hpp"
#include <stdexcept>

uint8 EspnowMessageAcknowledge::GetElementsSize()
{
    return sizeof(status);
}

EspnowMessageAcknowledge::EspnowMessageAcknowledge(Payload message)
{
    if(message.GetSize() != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    status = (bool)(message.data);
}

EspnowMessageAcknowledge::EspnowMessageAcknowledge(bool _status)
{
    status = _status;
}

bool EspnowMessageAcknowledge::GetStatus()
{
    return status;
}

Payload EspnowMessageAcknowledge::GetPayload() const
{
    Payload data(GetElementsSize());

    *(data.data) = status;

    Payload message(MessageTypeSize);
    *(message.data) = RSSI_CALCULATION;

    message += data;
    return message;
}
