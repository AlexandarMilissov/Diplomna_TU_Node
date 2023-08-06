#ifndef ESPNOW_MESSAGE_GENERAL_H_
#define ESPNOW_MESSAGE_GENERAL_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "Common.h"

typedef enum MessageType
{
    RSSI_REQUEST,
    RSSI_CALCULATION,
    RSSI_KEEP_ALIVE,
    RSSI_ACKNOWLEDGE
}MessageType;
#define MessageTypeSize 1

typedef struct Message
{
    ByteArray data;
    size_t data_size;
}Message;

Message* MessageInit(size_t);
void MessageDeinit(Message*);
void MessageDecompose(Message*, Message*, Message*);
Message* MessageCopy(Message*);
Message* MessageCompose(Message*, Message*);
void MessagePrintf(Message*);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ESPNOW_MESSAGE_GENERAL_H_