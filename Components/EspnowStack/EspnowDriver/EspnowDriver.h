#ifndef ESPNOWDRIVER_LOWER_H_
#define ESPNOWDRIVER_LOWER_H_

#include "Common.h"
#include "EspnowMessageGeneral.h"
#include "esp_now.h"

void EspnowDriver_Init(void(*callback)(const uint8_t*, const Message*, const RSSI_Type));
void DataSend(const uint8*, const Message*);
void DataReceive(const esp_now_recv_info_t*, const uint8_t*, int);

#endif // ESPNOWDRIVER_LOWER_H_