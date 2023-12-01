#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <vector>
#include "LoggerInterface.hpp"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

class LogManager
{
    private:
        static std::vector<LoggerInterface*> loggers;
    public:
        static void SetMinimalLevel(const char*, const Log_Severity);
        static void Log(const Log_Severity, const char*, const char*);
        static void Init(const void* pvParameters);
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // LOGGER_HPP_