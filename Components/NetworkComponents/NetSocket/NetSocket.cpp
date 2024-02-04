#include "NetSocket.hpp"
#include <cstring>

bool operator<(const NetSocket& left, const NetSocket& right)
{
    return memcmp(&left, &right, sizeof(NetSocket)) < 0;
}