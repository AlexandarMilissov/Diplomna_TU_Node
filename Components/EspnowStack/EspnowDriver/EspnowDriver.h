#ifndef ESPNOWDRIVER_LOWER_H_
#define ESPNOWDRIVER_LOWER_H_

#include "Common.h"
#include "Espnow_Message_General.h"
#include "esp_now.h"

void EspnowDriver_Init(void(*callback)(uint8_t*, Message*, RSSI_Type));
void DataSend(uint8*, Message*);
void DataReceive(const esp_now_recv_info_t*,const uint8_t*, int);

#endif // ESPNOWDRIVER_LOWER_H_