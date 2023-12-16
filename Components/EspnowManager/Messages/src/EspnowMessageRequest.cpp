#include "EspnowMessageRequest.hpp"
#include "EspnowManager.hpp"
#include <stdexcept>

// Functions for receiving messages
EspnowMessageRequest::EspnowMessageRequest(Payload message)
{
    if(message.GetSize() != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    subscriptionStatus = *((uint8*)message.data);
}

// Functions for sending messages
EspnowMessageRequest::EspnowMessageRequest(bool value)
{
    subscriptionStatus = value;
}
Payload EspnowMessageRequest::GetPayload() const
{
    Payload data(GetElementsSize());
    *(data.data) = subscriptionStatus;

    Payload message(MessageTypeSize);
    *(message.data) = RSSI_REQUEST;

    message += data;
    return message;
}

// Other functions

uint8 EspnowMessageRequest::GetElementsSize()
{
    return sizeof(subscriptionStatus);
}

EspnowMessageRequest::~EspnowMessageRequest()
{
}

bool EspnowMessageRequest::GetSubscriptionStatus()
{
    return subscriptionStatus;
}
