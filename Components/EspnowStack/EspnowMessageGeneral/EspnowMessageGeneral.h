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

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ESPNOWMESSAGEGENERAL_H_