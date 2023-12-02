#include "EspnowMessageKeepAlive.hpp"
#include "EspnowManager_Communication.hpp"

uint8 EspnowMessageKeepAlive::GetElementsSize()
{
    return 0;
}

// On receive
EspnowMessageKeepAlive::EspnowMessageKeepAlive(Payload Payload)
{
    DUMMY_STATEMENT(Payload);
}

EspnowMessageKeepAlive::EspnowMessageKeepAlive()
{
}

Payload EspnowMessageKeepAlive::GetPayload() const
{
    Payload header(MessageTypeSize);
    *(header.data) = RSSI_KEEP_ALIVE;

    return header;
}
