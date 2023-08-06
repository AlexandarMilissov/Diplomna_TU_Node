#include "To_C_Encapsulation.h"
#include "EspnowManager.h"
#include "EspnowDriver.h"


void MessageSend(uint8* dst_addr, MessageType messageType, Message* data)
{
    Message* header = MessageInit(MessageTypeSize);
    *(header->data) = messageType;

    Message* message = MessageCompose(header, data);

    DataSend(dst_addr, message);

    MessageDeinit(message);
    MessageDeinit(header);
}

void ManagerSubscribe()
{
    AddCalculationSubsciber();
}

void ManagerUnsubscribe()
{
    RemoveCalculationSubsciber();
}