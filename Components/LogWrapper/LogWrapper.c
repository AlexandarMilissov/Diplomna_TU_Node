#include "LogWrapper.h"
#include <stdarg.h>

#include "Common.h"

#include "Esp_LogWrapper.h"

char* LogWrapper_FormatString(const char* format, va_list args);

void LogWrapper_SetMinimalLevel(const char* source, const Log_Severity severity)
{
    Esp_LogWrapper_SetMinimalLevel(source, severity);
}

// Wrapper function
void LogWrapper_Log(const Log_Severity severity, const char* source, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // Format the log message
    char* message = LogWrapper_FormatString(format, args);

    // Call the actual logging function
    Esp_LogWrapper_Log(severity, source, message);

    free(message);

    va_end(args);
}

// Function to format the log message
char* LogWrapper_FormatString(const char* format, va_list args)
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
