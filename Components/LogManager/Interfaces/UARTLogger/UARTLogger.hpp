#ifndef CONSOLELOGGER_HPP_
#define CONSOLELOGGER_HPP_

#include "ILogger.hpp"

class UARTLogger : public ILogger
{
    public:
        void SetMinimalLevel(const char*, const LogSeverity);
        void Log(const LogSeverity, const char *, const char *);
};

#endif // CONSOLELOGGER_HPP_
