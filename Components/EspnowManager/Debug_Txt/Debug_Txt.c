#include "Debug_Txt.h"

void Debug_Txt_Msg_Received(void* vpmessage)
{
    Debug_Txt_Msg message = *((Debug_Txt_Msg*)vpmessage);
    printf("%s\n", message.msg);

    free(vpmessage);
}