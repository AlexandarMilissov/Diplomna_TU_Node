#ifndef TO_CPP_ENCAPSULATION_H
#define TO_CPP_ENCAPSULATION_H

#include <esp_now.h>


#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus

#include "Common.h"
#include "Espnow_Message_General.h"

void Send_Cyclic_Msg();
void SeriesSend();
void UpdateSeries();
void MessageReceive(uint8_t*, Message*, RSSI_Type);
void HandleReceivedMessages();
void To_CPP_Encapsulation_Init(void*);
size_t GetSeriersRepetitions();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_CPP_ENCAPSULATION_H