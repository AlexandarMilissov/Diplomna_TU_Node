#ifndef WIFIMANAGER_HPP
#define WIFIMANAGER_HPP

#include "Common.hpp"

class WifiManager
{
private:
    static bool isInit;
public:
    static void Init(const void*);
    static bool IsInit();
};

#endif