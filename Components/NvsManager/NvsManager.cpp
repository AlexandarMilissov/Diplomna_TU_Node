/**
 * @file NvsManager.cpp
 * @brief Implementation file for the NvsManager class.
 *
 * The NvsManager class provides functionality for initializing and accessing the Non-Volatile Storage (NVS) namespace.
 * It allows retrieving and setting valueKey-value pairs stored in the NVS, with support for various data types.
 */

#include "Common.hpp"
#include "NvsManager.hpp"
#include <string>
#include <stdexcept>

void NvsManager::Init()
{
    logManager.SetMinimalLevel("NvsManager", W);

    // Initialize NVS
    esp_err_t err;

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = nvs_open("Info", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        logManager.Log(E, "NvsManager", "Error opening NVS namespace: %s", esp_err_to_name(err));
        throw std::runtime_error("Error opening NVS namespace.");
    }
}

nvs_handle_t* NvsManager::GetHandle(std::string handleKey)
{
    nvs_handle_t* handle = nullptr;
    auto it = handlesMap.find(handleKey);
    if (it != handlesMap.end()) {
        handle = it->second;
    } else {
        handle = new nvs_handle_t;
        handlesMap[handleKey] = handle;
    }

    esp_err_t err = nvs_open(handleKey.c_str(), NVS_READWRITE, handle);
    if (err != ESP_OK)
    {
        std::string err_msg = std::string("Error opening NVS namespace: ") + esp_err_to_name(err);
        logManager.Log(E, "NvsManager", err_msg);
        throw std::runtime_error(err_msg);
    }
    return handle;
}

void NvsManager::SetVarErrorCheck(esp_err_t err)
{
    if(ESP_OK != err)
    {
        std::string err_msg = std::string("Error setting value to NVS: ") + esp_err_to_name(err);
        logManager.Log(E, "NvsManager", err_msg.c_str());
        throw std::runtime_error(err_msg);
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        std::string err_msg = std::string("Error committing value to NVS: ") + esp_err_to_name(err);
        logManager.Log(E, "NvsManager", err_msg.c_str());
        throw std::runtime_error(err_msg);
    }
}

bool NvsManager::GetVarErrorCheck(esp_err_t err)
{
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            return true;
        }
        else
        {
            std::string err_msg = std::string("Error getting value from NVS: ") + esp_err_to_name(err);
            logManager.Log(E, "NvsManager", err_msg.c_str());
            throw std::runtime_error(err_msg);
        }
    }
    return false;
}

template <>
void NvsManager::SetVar<uint8>(std::string valueKey, uint8* value)
{
    esp_err_t err = nvs_set_u8(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<uint16>(std::string valueKey, uint16* value)
{
    esp_err_t err = nvs_set_u16(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<uint32>(std::string valueKey, uint32* value)
{
    esp_err_t err = nvs_set_u32(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<uint64>(std::string valueKey, uint64* value)
{
    esp_err_t err = nvs_set_u64(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<sint8>(std::string valueKey, sint8* value)
{
    esp_err_t err = nvs_set_i8(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<sint16>(std::string valueKey, sint16* value)
{
    esp_err_t err = nvs_set_i16(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<sint32>(std::string valueKey, sint32* value)
{
    esp_err_t err = nvs_set_i32(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<sint64>(std::string valueKey, sint64* value)
{
    esp_err_t err = nvs_set_i64(my_handle, valueKey.c_str(), *value);
    SetVarErrorCheck(err);
}

template <>
void NvsManager::SetVar<std::string>(std::string valueKey, std::string* value)
{
    esp_err_t err = nvs_set_str(my_handle, valueKey.c_str(), value->c_str());
    SetVarErrorCheck(err);
}

template <>
uint8 NvsManager::GetVar<uint8>(std::string valueKey, uint8 defaultValue)
{
    uint8 value;
    esp_err_t err = nvs_get_u8(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<uint8>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
uint16 NvsManager::GetVar<uint16>(std::string valueKey, uint16 defaultValue)
{
    uint16 value;
    esp_err_t err = nvs_get_u16(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<uint16>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
uint32 NvsManager::GetVar<uint32>(std::string valueKey, uint32 defaultValue)
{
    uint32 value;
    esp_err_t err = nvs_get_u32(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<uint32>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
uint64 NvsManager::GetVar<uint64>(std::string valueKey, uint64 defaultValue)
{
    uint64 value;
    esp_err_t err = nvs_get_u64(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<uint64>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
sint8 NvsManager::GetVar<sint8>(std::string valueKey, sint8 defaultValue)
{
    sint8 value;
    esp_err_t err = nvs_get_i8(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<sint8>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
sint16 NvsManager::GetVar<sint16>(std::string valueKey, sint16 defaultValue)
{
    sint16 value;
    esp_err_t err = nvs_get_i16(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<sint16>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
sint32 NvsManager::GetVar<sint32>(std::string valueKey, sint32 defaultValue)
{
    sint32 value;
    esp_err_t err = nvs_get_i32(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<sint32>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
sint64 NvsManager::GetVar<sint64>(std::string valueKey, sint64 defaultValue)
{
    sint64 value;
    esp_err_t err = nvs_get_i64(my_handle, valueKey.c_str(), &value);

    if(GetVarErrorCheck(err))
    {
        SetVar<sint64>(valueKey, &defaultValue);
        value = defaultValue;
    }
    return value;
}

template <>
std::string NvsManager::GetVar<std::string>(std::string valueKey, std::string defaultValue)
{
    std::string value;
    size_t required_size;
    esp_err_t err = nvs_get_str(my_handle, valueKey.c_str(), NULL, &required_size);

    if(ESP_OK == err)
    {
        char* char_str = (char*)malloc(required_size);
        if(char_str == NULL)
        {
            err = ESP_ERR_NO_MEM;
        }
        else
        {
            err = nvs_get_str(my_handle, valueKey.c_str(), char_str, &required_size);
            if(ESP_OK == err)
            {
                value = std::string(char_str);
            }
            free(char_str);
        }
    }

    if(GetVarErrorCheck(err))
    {
        SetVar<std::string>(valueKey, &defaultValue);
        value = defaultValue;
    }

    return value;
}
