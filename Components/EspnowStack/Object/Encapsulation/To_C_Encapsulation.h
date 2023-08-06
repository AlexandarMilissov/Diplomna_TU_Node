#ifndef TO_C_ENCAPSULATION_HPP
#define TO_C_ENCAPSULATION_HPP

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus

#include "Common.h"
#include "Espnow_Message_General.h"
#include "TaskManager.h"

void MessageSend(uint8*, MessageType, Message*);
void ManagerSubscribe();
void ManagerUnsubscribe();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_C_ENCAPSULATION_HPP