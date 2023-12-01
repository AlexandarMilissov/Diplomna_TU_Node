#ifndef ESPNOWMESSAGEGENERAL_H_
#define ESPNOWMESSAGEGENERAL_H_

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
    uint8_t* data;
    size_t data_size;
}Message;

Message* MessageInit(const size_t);
void MessageDeinit(Message*);
Message* MessageCompose(const Message*,const Message*);
void MessageDecompose(const Message*, Message*, Message*);
Message* MessageCopy(const Message*);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ESPNOWMESSAGEGENERAL_H_