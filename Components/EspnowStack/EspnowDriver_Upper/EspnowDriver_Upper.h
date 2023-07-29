#ifndef ESPNOWDRIVER_UPPER_H_
#define ESPNOWDRIVER_UPPER_H_

#include "Common.h"

#include "espnow.h"
#include "espnow_utils.h"
#include "espnow_storage.h"

typedef enum MessageType
{
    RSSI_REQUEST,
    RSSI_CALCULATION,
    RSSI_KEEP_ALIVE,
    RSSI_ACKNOWLEDGE,
    DEBUG_TXT = 250
}MessageType;

typedef struct MessageStruct
{
    uint32 messageCouter;       // 4
    MessageType messageType;    // 1
    uint8 messageSize;          // 1
    void* message;              // 4
}MessageStruct;

esp_err_t DataReceive(uint8_t*, void*, size_t, wifi_pkt_rx_ctrl_t*);
esp_err_t DataSend(uint8_t*, MessageStruct);

uint8* MessageConstruct(MessageStruct, size_t*);
MessageStruct* MessageDeconstruct(uint8*, size_t);

#endif // ESPNOWDRIVER_UPPER_H_