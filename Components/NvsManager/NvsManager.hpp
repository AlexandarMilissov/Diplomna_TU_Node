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

class NvsManager
{
private:
    /* data */
public:
    static void Init(const void*);
    static const char* NvsGetName();
};


#endif // NVSMANAGER_H_