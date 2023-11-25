#ifndef TO_CPP_ENCAPSULATION_H
#define TO_CPP_ENCAPSULATION_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "Common.h"
#include "EspnowMessageGeneral.h"
#include <esp_now.h>

void Send_Cyclic_Msg();
void SeriesSend();
void UpdateSeries();
void MessageReceive(const uint8_t*, const Message*, const RSSI_Type);
void HandleReceivedMessages();
size_t GetSeriesRepetitions();
#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR
void To_CPP_Encapsulation_Init(void*);
#endif


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_CPP_ENCAPSULATION_H