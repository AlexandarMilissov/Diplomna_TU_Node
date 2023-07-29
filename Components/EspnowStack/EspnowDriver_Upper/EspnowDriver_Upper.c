#include "EspnowManager.h"
#include "EspnowDriver_Upper.h"

#include "Debug_Txt.h"
#include "To_CPP_Encapsulation.hpp"

extern State state;

esp_err_t DataReceive(uint8_t* src_addr, void* pvmessage, size_t size, wifi_pkt_rx_ctrl_t* rx_ctrl)
{
    if(state != RUN)
    {
        return ESP_ERR_ESPNOW_NOT_INIT;
    }
    
    MessageStruct* message = MessageDeconstruct((uint8*)pvmessage, size);

    switch (message->messageType)
    {
    case RSSI_REQUEST:
    case RSSI_CALCULATION:
    case RSSI_KEEP_ALIVE:
    case RSSI_ACKNOWLEDGE:
    {
        RSSI_OnMessageReceive(src_addr, message, size, rx_ctrl);
    }
        break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t DataSend(uint8_t* dst_addr, MessageStruct message)
{
    size_t size;

    uint8* data = MessageConstruct(message, &size);

    static espnow_frame_head_t frame_head = {
        .retransmit_count = 1,
        .broadcast        = true,
    };
    if(ESPNOW_ADDR_BROADCAST == dst_addr)
    {
        frame_head.broadcast = true;
    }
    else
    {
        frame_head.broadcast = false;
    }

    esp_err_t ret = espnow_send(ESPNOW_DATA_TYPE_DATA, dst_addr, data, size, &frame_head, portMAX_DELAY);
    
    free(data);
    return ret;
}

uint8* MessageConstruct(MessageStruct message, size_t* out_size)
{
    *out_size = 0;
    *out_size += sizeof(MessageStruct);       // The size of the sturct;
    *out_size -= sizeof(void*);         // Minus the size of the pointer
    *out_size += message.messageSize;   // Plus the size of the payload

    uint8* out = (uint8*)malloc(*out_size);    // Allocate the needed space
    
    *out_size -= message.messageSize;   // Minus the payload to calculate the start addres of the payload

    memcpy(out, &message, *out_size);   //Copy the info of the message
    memcpy(out + *out_size, message.message, message.messageSize);  //Copy the data of the message

    *out_size += message.messageSize;   // Plus the size of the payload

    return out;
}

MessageStruct* MessageDeconstruct(uint8* in, size_t in_size)
{
    uint8 header_len = sizeof(MessageStruct) - sizeof(void*);

    MessageStruct* message = (MessageStruct*)malloc(sizeof(MessageStruct));   // Allocate message space
    uint8* data = (uint8*)malloc(in_size - header_len);     // Allocate data space

    memcpy(message, in, header_len);                        // Copy the info of the message
    memcpy(data, in + header_len, in_size - header_len);    // Copy the ifo of the data

    message->messageSize = in_size - header_len;
    message->message = data;

    return message;
}
