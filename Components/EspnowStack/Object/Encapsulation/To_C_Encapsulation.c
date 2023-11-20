#include "To_C_Encapsulation.h"
#include "EspnowManager.h"
#include "EspnowDriver.h"


void MessageSend(const uint8* dst_addr, MessageType messageType, const Message* data)
{
    Message* header = MessageInit(MessageTypeSize);
    *(header->data) = (uint8)messageType;

    Message* message = MessageCompose(header, data);

    DataSend(dst_addr, message);

    MessageDeinit(message);
    MessageDeinit(header);
}

void ManagerSubscribe()
{
    AddCalculationSubscriber();
}

void ManagerUnsubscribe()
{
    RemoveCalculationSubscriber();
}