#include "RSSI_Message_Request.hpp"
#include "EspnowManager_Communication.hpp"
#include <stdexcept>

// Functions for receiving messages
RSSI_Message_Request::RSSI_Message_Request(Message message)
{
    if(message.data_size != GetElementsSize())
    {
        throw std::invalid_argument("Wrong message size");
    }
    subscriptionStatus = *((uint8*)message.data);
}

// Functions for sending messages
RSSI_Message_Request::RSSI_Message_Request(bool value)
{
    subscriptionStatus = value;
}

void RSSI_Message_Request::Send(uint8* dst_addr)
{
    Message* message = MessageInit(GetElementsSize());
    *(message->data) = subscriptionStatus;

    SendMessage(dst_addr, RSSI_REQUEST, message);

    MessageDeinit(message);
}

// Other functions

uint8 RSSI_Message_Request::GetElementsSize()
{
    return sizeof(subscriptionStatus);
}

RSSI_Message_Request::~RSSI_Message_Request()
{
}

bool RSSI_Message_Request::GetSubscriptionStatus()
{
    return subscriptionStatus;
}
