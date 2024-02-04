#ifndef NETSOCKET_HPP_
#define NETSOCKET_HPP_

#include "Common.hpp"

typedef struct
{
    uint32 ip;
    uint16 port;
}NetSocket;

bool operator<(const NetSocket&, const NetSocket&);

#endif // NETSOCKET_HPP_
