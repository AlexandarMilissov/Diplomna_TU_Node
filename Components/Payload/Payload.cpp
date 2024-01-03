#include "Payload.hpp"
#include <string.h>
#include <stdexcept>

Payload::Payload(const Payload& original) : size(original.size)
{
    if(size > 0)
    {
        data = (uint8*)malloc(sizeof(uint8) * size);
        memcpy(data, original.data, size);
    }
}

Payload::Payload(const size_t size) : size(size)
{
    if(size > 0)
    {
        data = (uint8*)malloc(sizeof(uint8) * size);
    }
}

Payload::Payload(const uint8* newData, const size_t size) : size(size)
{
    if(size > 0)
    {
        data = (uint8*)malloc(sizeof(uint8) * size);
        memcpy(data, newData, size);
    }
}

Payload::Payload(const MacAddress& mac) : size(MAC_ADDRESS_LENGTH)
{
    data = (uint8*)malloc(sizeof(uint8) * size);
    mac.CopyTo(data);
}

Payload::~Payload()
{
    if(size > 0)
    {
        free(data);
    }
}

Payload& Payload::operator+=(const Payload &add)
{
    if(add.size > 0)
    {
        data = (uint8*)realloc(data, size + add.size);
        memcpy(data + size, add.data, add.size);

        size += add.size;
    }
    return *this;
}

Payload& Payload::operator>>=(Payload &sub)
{
    if(size > sub.size)
    {
        uint8* temp;
        temp = (uint8*)malloc(size - sub.size);

        memcpy(sub.data, data, sub.size);
        size -= sub.size;
        memcpy(temp, data + sub.size, size);

        free(data);
        data = temp;
    }
    else if (size == sub.size)
    {
        memcpy(sub.data, data, size);
        free(data);
        size = 0;
    }
    else
    {
        throw std::invalid_argument("Cannot extract payload bigger than the current one.");
    }

    return *this;
}

size_t Payload::GetSize() const
{
    return size;
}
