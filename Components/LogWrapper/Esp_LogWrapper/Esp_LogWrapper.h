#ifndef ESP_LOGWRAPPER_H_
#define ESP_LOGWRAPPER_H_

#include "LogWrapper.h"
#include <stdarg.h>

void Esp_LogWrapper_SetMinimalLevel(const char*, const Log_Severity);
void Esp_LogWrapper_Log(const Log_Severity, const char*, const char*);

#endif // ESP_LOGWRAPPER_H_