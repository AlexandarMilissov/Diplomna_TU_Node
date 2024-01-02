#ifndef NVSMANAGER_HPP_
#define NVSMANAGER_HPP_

#include "IComponent.hpp"
#include "LogManager.hpp"

#include "nvs_flash.h"

#include <map>

class NvsManager : public IComponent
{
private:
    nvs_handle_t my_handle;
    LogManager& logManager;
    std::map<std::string, nvs_handle_t*> handlesMap;

    void SetVarErrorCheck(esp_err_t);
    bool GetVarErrorCheck(esp_err_t);

public:
    NvsManager(LogManager& logManager) : logManager(logManager) {};

    nvs_handle_t* GetHandle(std::string);

    void Init();

    template <typename NvsValueType>
    void SetVar(std::string, NvsValueType*);

    template <typename NvsValueType>
    NvsValueType GetVar(std::string, NvsValueType = NvsValueType());
};

#endif // NVSMANAGER_HPP_
