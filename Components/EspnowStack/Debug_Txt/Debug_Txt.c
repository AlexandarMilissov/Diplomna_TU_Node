#include "Debug_Txt.h"

void Debug_Txt_Msg_Received(Debug_Txt_Msg message)
{
    printf("%s\n", message.msg);
}