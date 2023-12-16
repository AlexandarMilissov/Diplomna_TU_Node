#ifndef ESPNOWMANAGER_COMMUNICATION_HPP
#define ESPNOWMANAGER_COMMUNICATION_HPP

#include "Common.hpp"
#include "Payload.hpp"

void SendMessage(const uint8*, const Payload);

void ReceiveMessage(const uint8_t*, const Payload*, const RSSI_Type);
void HandleReceivedMessages();


#endif
