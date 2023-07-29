#ifndef TO_C_ENCAPSULATION_HPP
#define TO_C_ENCAPSULATION_HPP

#ifdef __cplusplus
extern "C" 
{
#endif //__cplusplus

#include "Common.h"
#include "EspnowDriver_Upper.h"
#include "TaskManager.h"

void MessageSend(MessageType, uint8, void*);
void ManagerSubscribe();
void ManagerUnsubscribe();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_C_ENCAPSULATION_HPP