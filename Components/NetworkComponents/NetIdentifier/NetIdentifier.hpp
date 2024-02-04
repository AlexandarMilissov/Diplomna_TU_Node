#ifndef NETIDENTIFIER_HPP_
#define NETIDENTIFIER_HPP_

#include "MacAddress.hpp"
#include "NetSocket.hpp"
#include "Common.hpp"

typedef union
{
    uint8 mac[6];
    uint32 ip;
    NetSocket socket;
}NetIdentifier;

#endif // NETIDENTIFIER_HPP_
