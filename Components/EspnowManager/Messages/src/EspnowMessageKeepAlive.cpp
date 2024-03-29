#include "EspnowMessageKeepAlive.hpp"
#include "EspnowManager.hpp"
#include "Messages.hpp"

EspnowMessageKeepAlive::EspnowMessageKeepAlive(std::queue<Payload> payload)
{
    DUMMY_STATEMENT(payload);
}

EspnowMessageKeepAlive::EspnowMessageKeepAlive()
{
}

std::stack<Payload> EspnowMessageKeepAlive::GetPayload() const
{
    MessageType messageType = NOW_KEEP_ALIVE;
    Payload messageIdentifier((uint8*)(&messageType), sizeof(messageType));

    std::stack<Payload> payloadStack;
    payloadStack.push(messageIdentifier);

    return payloadStack;
}
