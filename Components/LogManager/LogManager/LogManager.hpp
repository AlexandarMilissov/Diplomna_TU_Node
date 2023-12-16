#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <vector>
#include <stdarg.h>
#include "LogSeverity.hpp"
#include "ILogger.hpp"

class LogManager
{
    private:
        static std::vector<ILogger*> loggers;
        static char* FormatString(const char*, va_list);
    public:
        static void SetMinimalLevel(const char*, const Log_Severity);
        static void Log(const Log_Severity, const char*, const char*, ...);
        static void Init(const void* pvParameters);
};


#endif // LOGGER_HPP_
