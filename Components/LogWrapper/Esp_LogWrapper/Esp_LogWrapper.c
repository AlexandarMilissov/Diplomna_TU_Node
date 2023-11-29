#include "Esp_LogWrapper.h"
#include <esp_log.h>
#include <stdio.h>
#include "Common.h"

void Esp_LogWrapper_SetMinimalLevel(const char* source, const Log_Severity severity)
{
    switch (severity)
    {
    case V:
        esp_log_level_set(source, ESP_LOG_VERBOSE);
        break;
    case D:
        esp_log_level_set(source, ESP_LOG_DEBUG);
        break;
    case I:
        esp_log_level_set(source, ESP_LOG_INFO);
        break;
    case W:
        esp_log_level_set(source, ESP_LOG_WARN);
        break;
    case E:
        esp_log_level_set(source, ESP_LOG_ERROR);
        break;
    default:
        break;
    }
}

void Esp_LogWrapper_Log(const Log_Severity severity, const char* source, const char* log)
{
    switch (severity)
    {
    case V:
        ESP_LOGV(source, "%s", log);
        break;
    case D:
        ESP_LOGD(source, "%s", log);
        break;
    case I:
        ESP_LOGI(source, "%s", log);
        break;
    case W:
        ESP_LOGW(source, "%s", log);
        break;
    case E:
        ESP_LOGE(source, "%s", log);
        break;
    default:
        break;
    }
}
