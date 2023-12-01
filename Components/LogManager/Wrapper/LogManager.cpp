#include "LogManager.h"
#include "LogManager.hpp"
#include <stdarg.h>

#include "Common.h"

char* Logger_FormatString(const char* format, va_list args);

void LogManager_Init(const void* pvParameters)
{
    LogManager::Init(pvParameters);
}

void LogManager_SetMinimalLevel(const char* source, const Log_Severity severity)
{
    LogManager::SetMinimalLevel(source, severity);
}

// Wrapper function
void LogManager_Log(const Log_Severity severity, const char* source, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // Format the log message
    char* message = Logger_FormatString(format, args);

    // Call the actual logging function
    LogManager::Log(severity, source, message);

    free(message);

    va_end(args);
}

// Function to format the log message
char* Logger_FormatString(const char* format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    // Determine the length of the formatted message
    int length = vsnprintf(NULL, 0, format, args_copy);

    // Allocate memory for the formatted message
    char* buffer = (char*)malloc(length + 1); // +1 for the null terminator
    buffer[length] = '\0';

    // Format the message into the allocated buffer
    vsnprintf(buffer, length + 1, format, args);

    va_end(args_copy);

    return buffer;
}