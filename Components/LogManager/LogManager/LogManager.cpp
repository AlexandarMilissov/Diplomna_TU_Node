#include "LogManager.hpp"
#include "LogManager.hpp"
#include "Common.hpp"
#include "UARTLogger.hpp"

std::vector<ILogger*> LogManager::loggers;

void LogManager::Init(const void *pvParameters)
{
    DUMMY_STATEMENT(pvParameters);

    loggers.push_back(new UARTLogger());
}

void LogManager::SetMinimalLevel(const char* source, const Log_Severity severity)
{
    for (auto logger : loggers)
    {
        logger->SetMinimalLevel(source, severity);
    }
}

void LogManager::Log(const Log_Severity severity, const char * source, const char * format, ...)
{
    va_list args;
    va_start(args, format);

    // Format the log message
    char* message = FormatString(format, args);

    // Call the actual logging functions
    for (auto logger : loggers)
    {
        logger->Log(severity, source, message);
    }

    free(message);

    va_end(args);
}

// Function to format the log message
char* LogManager::FormatString(const char* format, va_list args)
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