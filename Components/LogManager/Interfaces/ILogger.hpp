#ifndef LOGGERINTERFACE_HPP_
#define LOGGERINTERFACE_HPP_

#include "LogManager.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

class ILogger
{
    public:
        virtual void SetMinimalLevel(const char*, const Log_Severity) = 0;
        virtual void Log(const Log_Severity, const char*, const char*) = 0;
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // LOGGERINTERFACE_HPP_