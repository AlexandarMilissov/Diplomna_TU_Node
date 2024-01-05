#ifndef MACADDRESS_HPP_
#define MACADDRESS_HPP_

#include "Common.hpp"
#include <string>

class Payload;

#define MAC_ADDRESS_LENGTH 6
class MacAddress
{
private:
    uint8 address[6];
public:
    MacAddress();
    MacAddress(uint8*);
    MacAddress(const Payload&);

    void CopyTo(uint8* destination) const;
    uint8* GetAddress() const;
    void SetAddress(uint8*);

    bool operator==(const MacAddress& other) const;
    bool operator==(const uint8* other) const;
    bool operator!=(const MacAddress& other) const;
    bool operator!=(const uint8* other) const;

    std::string ToString() const;
};


#endif // MACADDRESS_HPP_