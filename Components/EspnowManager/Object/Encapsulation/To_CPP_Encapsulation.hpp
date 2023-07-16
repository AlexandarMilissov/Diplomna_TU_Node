#ifndef TO_CPP_ENCAPSULATION_H
#define TO_CPP_ENCAPSULATION_H

#include <esp_now.h>

#include "Espnow_Message_General.h"

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus


void Send_Keep_Alive_Msg();
esp_err_t RSSI_MessageReceive(uint8_t*, MessageStruct, size_t, wifi_pkt_rx_ctrl_t*);
void To_CPP_Encapsulation_Init(void*);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_CPP_ENCAPSULATION_H