#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <vector>
#include <stdarg.h>
#include <string>
#include "LogSeverity.hpp"
#include "ILogger.hpp"
#include "BaseComponent.hpp"

class LogManager : public BaseComponent
{
    private:
        std::vector<ILogger*> loggers;
        static char* FormatString(const char*, va_list);
    public:
        void Init();
        void SetMinimalLevel(const char*, const LogSeverity);
        void Log(const LogSeverity, const char*, const char*, ...);
        void Log(const LogSeverity, const char*, const std::string);
};

#endif // LOGGER_HPP_
