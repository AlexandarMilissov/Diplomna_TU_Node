#include "Espnow_Message_General.h"
#include <string.h>

Message* MessageInit(size_t data_size)
{
    Message* out = (Message*)malloc(sizeof(Message));
    ByteArray data = (ByteArray)malloc(sizeof(uint8) * data_size);
    out->data = data;
    out->data_size = data_size;

    return out;
}
void MessageDeinit(Message* in)
{
    free(in->data);
    free(in);
}

void MessageDecompose(Message* in, Message* out_header, Message* out_data)
{
    // Check for input errors
    if(out_header->data_size > in->data_size)
    {
        ESP_LOGE("MessageDecompose","Invalid data size");
    }

    // Fill header info
    memcpy(out_header->data, in->data, out_header->data_size);

    // Fill data info
    out_data->data_size = in->data_size - out_header->data_size;
    if(out_data->data_size != 0)
    {
        out_data->data = realloc(out_data->data, out_data->data_size);
        memcpy(out_data->data, in->data + out_header->data_size, out_data->data_size);
    }
}

Message* MessageCopy(Message* original)
{
    if(original == NULL)
    {
        return NULL;
    }

    Message* copy = MessageInit(original->data_size);
    memcpy(copy->data, original->data, original->data_size);

    return copy;
}

Message* MessageCompose(Message* in_header, Message* in_data)
{
    Message* out;

    if(in_data == NULL)
    {
        out = MessageCopy(in_header);
    }
    else
    {
        out = MessageInit(in_header->data_size + in_data->data_size);
        memcpy(out->data,                           in_header->data,    in_header->data_size);
        memcpy(out->data + in_header->data_size,    in_data->data,      in_data  ->data_size);
    }

    return out;
}

void MessagePrintf(Message* message)
{
    printf("Message: ");
    for (int i = 0; i < message->data_size; i++)
    {
        printf("%d ",message->data[i]);
    }
    printf("\n");
}
