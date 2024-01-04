#include "Payload.hpp"
#include <string.h>
#include <stdexcept>
#include <cstring>
#include "MacAddress.hpp"

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
    if(sub.size > 0)
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
    }

    return *this;
}

size_t Payload::GetSize() const
{
    return size;
}

size_t Payload::GetSizeOfSize()
{
    return sizeof(size);
}

Payload Payload::Compose(std::stack<Payload> payloadStack)
{
    size_t len = 0;
    uint8* data = NULL;

    while(!payloadStack.empty())
    {
        // Get the payload from the stack
        Payload payload = payloadStack.top();
        payloadStack.pop();

        // Get the size of the payload
        size_t payloadSize = payload.GetSize();
        static size_t sizeOfPayloadSize = Payload::GetSizeOfSize();

        // Allocate memory for the size of the payload
        data = (uint8*)realloc(data, len + sizeOfPayloadSize);

        // Copy the size of the payload to the data
        std::memcpy(data + len, &payloadSize, sizeOfPayloadSize);

        // Increase the length
        len += sizeOfPayloadSize;

        // Allocate memory for the data
        data = (uint8*)realloc(data, len + payloadSize);

        // Copy the payload to the data
        std::memcpy(data + len, payload.data, payloadSize);

        // Increase the length
        len += payloadSize;
    }

    Payload ret(data, len);
    free(data);
    return ret;
}

std::queue<Payload> Payload::Decompose(const Payload& data)
{
    std::queue<Payload> payloadQueue;
    uint8* dataPointer = data.data;
    size_t payloadLen;
    size_t len = data.GetSize();
    while(len > 0)
    {
        static size_t sizeOfPayloadSize = Payload::GetSizeOfSize();

        if(len <= sizeOfPayloadSize)
        {
            std::string E = "Received message with broken size.";
            throw std::invalid_argument(E);
        }

        payloadLen = 0;
        memcpy(&payloadLen, dataPointer, sizeOfPayloadSize);
        dataPointer += sizeOfPayloadSize;
        len -= sizeOfPayloadSize;

        if(len < payloadLen)
        {
            std::string E = "Received message with broken size.";
            throw std::invalid_argument(E);
        }

        payloadQueue.push(Payload(dataPointer, payloadLen));
        dataPointer += payloadLen;

        len -= payloadLen;
    }

    if(len != 0)
    {
        throw std::invalid_argument("Sanity check failed.");
    }

    return payloadQueue;
}
