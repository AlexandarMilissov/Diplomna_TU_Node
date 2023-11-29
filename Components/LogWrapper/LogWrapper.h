#ifndef LOGGER_H_
#define LOGGER_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef enum{
    V,
    D,
    I,
    W,
    E
}Log_Severity;

void LogWrapper_SetMinimalLevel(const char*, const Log_Severity);
void LogWrapper_Log(const Log_Severity, const char*, const char*, ...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // LOGGER_H_