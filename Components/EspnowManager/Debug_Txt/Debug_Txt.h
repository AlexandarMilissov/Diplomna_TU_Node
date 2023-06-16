#ifndef DEBUG_TXT_H
#define DEBUG_TXT_H

#include "Common.h"

typedef struct Debug_Txt_Msg
{
    char msg[240];
}Debug_Txt_Msg;

void Debug_Txt_Msg_Received(void*);

#endif