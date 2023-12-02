#ifndef CONSOLELOGGER_HPP_
#define CONSOLELOGGER_HPP_

#include "ILogger.hpp"

class UARTLogger : public ILogger
{
    public:
        void SetMinimalLevel(const char*, const Log_Severity);
        void Log(const Log_Severity, const char *, const char *);
};

#endif // CONSOLELOGGER_HPP_
