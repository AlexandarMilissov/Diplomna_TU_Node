#ifndef ESPMANAGER_PRIVATE_H
#define ESPMANAGER_PRIVATE_H

#include "Common.h"

#include "espnow.h"
#include "espnow_utils.h"
#include "espnow_storage.h"

typedef struct Message
{
    uint32 messageCouter; // 4
    uint8 messageId;      // 1
    uint8 messageSize;    // 1
    void* message;        // 4
}Message;                 

enum MessageType
{
    RSSI_REQUEST,
    RSSI_CALCULATION,
    RSSI_KEEP_ALIVE,
    RSSI_ACKNOWLEDGE,
    DEBUG_TXT = 250
};

typedef enum State{
    NO_INIT,
    INIT,
    RUN,
}State;


esp_err_t DataReceive(uint8_t*, void*, size_t, wifi_pkt_rx_ctrl_t*);
esp_err_t DataSend(uint8_t*, Message);

uint8* MessageConstruct(Message, size_t*);
Message* MessageDeconstruct(uint8*, size_t);

#endif