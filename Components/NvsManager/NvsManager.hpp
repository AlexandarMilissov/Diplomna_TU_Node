#ifndef NVSMANAGER_HPP_
#define NVSMANAGER_HPP_

#include "IComponent.hpp"
#include "LogManager.hpp"

#include "nvs_flash.h"

#include <map>

class NvsManager : public IComponent
{
private:
    LogManager& logManager;
    std::map<std::string, nvs_handle_t*> handlesMap;

    nvs_handle_t* GetHandle(std::string);
    void SetVarErrorCheck(esp_err_t, nvs_handle_t);
    bool GetVarErrorCheck(esp_err_t);

public:
    NvsManager(LogManager& logManager) : logManager(logManager) {};
    ~NvsManager();

    void Init();

    template <typename NvsValueType>
    void SetVar(std::string, std::string, NvsValueType*);

    template <typename NvsValueType>
    NvsValueType GetVar(std::string, std::string, NvsValueType = NvsValueType());

    void EraseVar(std::string, std::string);
    void EraseAll(std::string);
};

#endif // NVSMANAGER_HPP_
