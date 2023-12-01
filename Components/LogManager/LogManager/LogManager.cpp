#include "LogManager.h"
#include "LogManager.hpp"
#include "Common.h"
#include "UARTLogger.hpp"

std::vector<LoggerInterface*> LogManager::loggers;

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

void LogManager::Log(const Log_Severity severity, const char * source, const char * message)
{
    for (auto logger : loggers)
    {
        logger->Log(severity, source, message);
    }
}
