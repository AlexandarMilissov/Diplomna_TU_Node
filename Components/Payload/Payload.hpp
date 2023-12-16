#ifndef PAYLOAD_HPP_
#define PAYLOAD_HPP_

#include "Common.hpp"

class Payload
{
    private:
        size_t size;
    public:
        uint8* data;
        Payload(const Payload&);
        Payload(const size_t);
        Payload(const uint8*, const size_t);
        ~Payload();
        Payload& operator+=(const Payload&);
        Payload& operator>>=(Payload&);
        size_t GetSize() const;
};

#endif // PAYLOAD_HPP_
