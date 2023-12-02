#ifndef ESPNOWMANAGER_COMMUNICATION_H
#define ESPNOWMANAGER_COMMUNICATION_H

#include "Common.h"
#include "Payload.hpp"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

void SendMessage(const uint8*, const Payload);

void ReceiveMessage(const uint8_t*, const Payload*, const RSSI_Type);
void HandleReceivedMessages();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
