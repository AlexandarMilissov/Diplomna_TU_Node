#ifndef NVSMANAGER_H_
#define NVSMANAGER_H_

typedef enum NvsVarCode
{
    VAR8,
    VAR16,
    VAR32,
    VAR64
}NvsVarCode;

typedef enum NvsLargeCode
{
    STRING,
    BLOB
}NvsLargeCode;

void Hooks_Init(const void*);
void NvsManager_Init(const void*);
const char* NvsGetName();

#endif // NVSMANAGER_H_