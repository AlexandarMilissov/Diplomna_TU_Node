#ifndef ESPNOWDRIVER_HPP_
#define ESPNOWDRIVER_HPP_

#include "Common.hpp"
#include "Payload.hpp"
#include "esp_now.h"

void EspnowDriver_Init(void(*callback)(const uint8*, const Payload*, const RSSI_Type));
void DataSend(const uint8*, const Payload*);
void DataReceive(const esp_now_recv_info_t*, const uint8*, int);

#endif // ESPNOWDRIVER_HPP_
