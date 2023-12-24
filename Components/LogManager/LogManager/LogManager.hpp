#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <vector>
#include <stdarg.h>
#include "LogSeverity.hpp"
#include "ILogger.hpp"
#include "IComponent.hpp"

class LogManager : public IComponent
{
    private:
        std::vector<ILogger*> loggers;
        static char* FormatString(const char*, va_list);
    public:
        void Init();
        void SetMinimalLevel(const char*, const Log_Severity);
        void Log(const Log_Severity, const char*, const char*, ...);
};

#endif // LOGGER_HPP_
