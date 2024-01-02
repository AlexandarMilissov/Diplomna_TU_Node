#ifndef LOGGERINTERFACE_HPP_
#define LOGGERINTERFACE_HPP_

#include "LogSeverity.hpp"

class ILogger
{
    public:
        virtual void SetMinimalLevel(const char*, const LogSeverity) = 0;
        virtual void Log(const LogSeverity, const char*, const char*) = 0;
};


#endif // LOGGERINTERFACE_HPP_