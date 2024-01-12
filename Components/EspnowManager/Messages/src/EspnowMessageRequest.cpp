#include "EspnowMessageRequest.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>

// Functions for receiving messages
EspnowMessageRequest::EspnowMessageRequest(std::queue<Payload> payloadQueue)
{
    if(payloadQueue.empty())
    {
        throw std::invalid_argument(std::string("Payload queue is empty!\n"));
    }

    Payload message = payloadQueue.front();
    subscriptionStatus = *((bool*)message.GetData());
}

// Functions for sending messages
EspnowMessageRequest::EspnowMessageRequest(bool value)
{
    subscriptionStatus = value;
}

std::stack<Payload> EspnowMessageRequest::GetPayload() const
{
    Payload data((uint8*)(&subscriptionStatus), sizeof(subscriptionStatus));

    EspnowMessageType messageType = NOW_REQUEST;
    Payload message((uint8*)(&messageType), sizeof(messageType));

    std::stack<Payload> payloadStack;
    payloadStack.push(data);
    payloadStack.push(message);

    return payloadStack;
}

bool EspnowMessageRequest::GetSubscriptionStatus()
{
    return subscriptionStatus;
}
