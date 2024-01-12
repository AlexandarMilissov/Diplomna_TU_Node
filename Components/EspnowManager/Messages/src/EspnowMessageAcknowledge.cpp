#include "EspnowMessageAcknowledge.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>

EspnowMessageAcknowledge::EspnowMessageAcknowledge(std::queue<Payload> payloadQueue)
{
    if(payloadQueue.empty())
    {
        throw std::invalid_argument(std::string("Payload queue is empty!\n"));
    }

    Payload message = payloadQueue.front();
    status = (bool)(message.GetData());
}

EspnowMessageAcknowledge::EspnowMessageAcknowledge(bool _status)
{
    status = _status;
}

bool EspnowMessageAcknowledge::GetStatus()
{
    return status;
}

std::stack<Payload> EspnowMessageAcknowledge::GetPayload() const
{
    Payload data((uint8*)(&status), sizeof(status));

    EspnowMessageType messageType = NOW_ACKNOWLEDGE;
    Payload message((uint8*)(&messageType), sizeof(messageType));

    std::stack<Payload> payloadStack;
    payloadStack.push(data);
    payloadStack.push(message);

    return payloadStack;
}
