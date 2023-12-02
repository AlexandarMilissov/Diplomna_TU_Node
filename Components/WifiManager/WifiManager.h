#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"

void WifiManager_Init(const void*);
bool WifiManager_IsInit();

#ifdef __cplusplus
}
#endif

#endif