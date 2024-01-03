#ifndef MACADDRESS_HPP_
#define MACADDRESS_HPP_

#include "Common.hpp"

#define MAC_ADDRESS_LENGTH 6
class MacAddress
{
private:
    uint8 address[6];
public:
    MacAddress() {}
    MacAddress(uint8*);

    void CopyTo(uint8* destination) const;

    bool operator==(const MacAddress& other) const;
    bool operator!=(const MacAddress& other) const;
};


#endif // MACADDRESS_HPP_