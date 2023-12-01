#ifndef CONSOLELOGGER_HPP_
#define CONSOLELOGGER_HPP_

#include "LoggerInterface.hpp"

class UARTLogger : public LoggerInterface
{
    public:
        void SetMinimalLevel(const char*, const Log_Severity);
        void Log(const Log_Severity, const char *, const char *);
};

#endif // CONSOLELOGGER_HPP_
