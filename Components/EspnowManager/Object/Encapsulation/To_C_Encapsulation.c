#include "To_C_Encapsulation.h"

#include "Espnow_Message_General.h"

void MessageSend(MessageType messageType, uint8 data_size, void* data)
{
    MessageStruct RSSIMessage = {
        .messageCouter = 0,
        .messageType = messageType,
        .messageSize = data_size,
        .message = data,
    };

    DataSend((uint8_t*)ESPNOW_ADDR_BROADCAST, RSSIMessage);
}