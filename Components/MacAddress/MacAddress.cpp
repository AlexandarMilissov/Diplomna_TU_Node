#include "MacAddress.hpp"
#include <cstring>

#define MAC_ADDRESS_LENGTH 6

MacAddress::MacAddress(uint8* address)
{
    memcpy(this->address, address, MAC_ADDRESS_LENGTH);
}

void MacAddress::CopyTo(uint8* destination) const
{
    memcpy(destination, this->address, MAC_ADDRESS_LENGTH);
}

bool MacAddress::operator==(const MacAddress& other) const
{
    return memcmp(this->address, other.address, MAC_ADDRESS_LENGTH) == 0;
}

bool MacAddress::operator!=(const MacAddress& other) const
{
    return !(*this == other);
}
