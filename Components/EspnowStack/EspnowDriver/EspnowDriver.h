#ifndef ESPNOWDRIVER_LOWER_H_
#define ESPNOWDRIVER_LOWER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "EspnowMessageGeneral.h"
#include "esp_now.h"

void EspnowDriver_Init(void(*callback)(const uint8*, const Message*, const RSSI_Type));
void DataSend(const uint8*, const Message*);
void DataReceive(const esp_now_recv_info_t*, const uint8*, int);

#ifdef __cplusplus
}
#endif

#endif // ESPNOWDRIVER_LOWER_H_