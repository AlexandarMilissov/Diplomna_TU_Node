#ifndef TO_CPP_ENCAPSULATION_H
#define TO_CPP_ENCAPSULATION_H

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus

#include "Common.h"
#include "Espnow_Message_General.h"
#include <esp_now.h>

void Send_Cyclic_Msg();
void SeriesSend();
void EncapsulationMonitor();
void UpdateSeries();
void MessageReceive(const uint8_t*, const Message*, const RSSI_Type);
void HandleReceivedMessages();
void To_CPP_Encapsulation_Init(void*);
size_t GetSeriesRepetitions();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_CPP_ENCAPSULATION_H