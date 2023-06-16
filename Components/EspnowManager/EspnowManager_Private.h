#include "Common.h"

typedef struct Message
{
    uint32 messageCouter; // 4
    uint8 messageId;      // 1
    uint8 messageSize;    // 1
    void* message;        // 4 -> 254
}Message;                 // 250

enum MessageType
{
    RSSI_REQUEST,
    RSSI_CALCULATION,
    RSSI_KEEP_ALIVE,
    DEBUG_TXT = 250
};

typedef enum State{
    NO_INIT,
    INIT,
}State;


esp_err_t DataReceive(uint8_t*, void*, size_t, wifi_pkt_rx_ctrl_t*);
esp_err_t DataSend(uint8_t*, Message);

uint8* MessageConstruct(Message, size_t*);
Message* MessageDeconstruct(uint8*, size_t);