#ifndef NVSMANAGER_HPP_
#define NVSMANAGER_HPP_

#include "nvs_flash.h"
class NvsManager
{
private:
    static nvs_handle_t my_handle;
public:
    static void Init(const void*);
    template <typename NvsValueType>
    static NvsValueType GetVar(const char*);
    template <typename NvsValueType>
    void SetVar(const char*, NvsValueType*);
};

#endif // NVSMANAGER_HPP_
