#ifndef TO_C_ENCAPSULATION_HPP
#define TO_C_ENCAPSULATION_HPP

#include "TaskManager.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "Common.h"
#include "EspnowMessageGeneral.h"

void MessageSend(const uint8*, MessageType, const Message*);
void ManagerSubscribe();
void ManagerUnsubscribe();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TO_C_ENCAPSULATION_HPP