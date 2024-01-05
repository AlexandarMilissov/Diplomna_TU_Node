#include "MacAddress.hpp"
#include <cstring>
#include <stdexcept>

#include "Payload.hpp"

MacAddress::MacAddress()
{
    memset(this->address, 0, MAC_ADDRESS_LENGTH);
}

MacAddress::MacAddress(uint8* address)
{
    memcpy(this->address, address, MAC_ADDRESS_LENGTH);
}

MacAddress::MacAddress(const Payload& payload)
{
    if(payload.GetSize() != MAC_ADDRESS_LENGTH)
    {
        throw std::invalid_argument("Payload size does not match MAC address length.");
    }
    memcpy(this->address, payload.data, MAC_ADDRESS_LENGTH);
}

void MacAddress::CopyTo(uint8* destination) const
{
    memcpy(destination, this->address, MAC_ADDRESS_LENGTH);
}

uint8* MacAddress::GetAddress() const
{
    return (uint8*)this->address;
}

void MacAddress::SetAddress(uint8* address)
{
    memcpy(this->address, address, MAC_ADDRESS_LENGTH);
}

bool MacAddress::operator==(const MacAddress& other) const
{
    return memcmp(this->address, other.address, MAC_ADDRESS_LENGTH) == 0;
}

bool MacAddress::operator==(const uint8* other) const
{
    return memcmp(this->address, other, MAC_ADDRESS_LENGTH) == 0;
}

bool MacAddress::operator!=(const MacAddress& other) const
{
    return !(*this == other);
}

bool MacAddress::operator!=(const uint8* other) const
{
    return !(*this == other);
}

std::string MacAddress::ToString() const
{
    std::string addressString = "";
    addressString += std::to_string(address[0]) + ":";
    addressString += std::to_string(address[1]) + ":";
    addressString += std::to_string(address[2]) + ":";
    addressString += std::to_string(address[3]) + ":";
    addressString += std::to_string(address[4]) + ":";
    addressString += std::to_string(address[5]);
    return addressString;
}
