#ifndef TO_CPP_ENCAPSULATION_H
#define TO_CPP_ENCAPSULATION_H

#include <esp_now.h>

#include "Espnow_Message_General.h"

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus


void Send_Cyclic_Msg();
void SeriesSend();
void UpdateSeries();
esp_err_t RSSI_OnMessageReceive(uint8_t*, MessageStruct*, size_t, wifi_pkt_rx_ctrl_t*);
void HandleReceivedMessages();
void To_CPP_Encapsulation_Init(void*);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_CPP_ENCAPSULATION_H