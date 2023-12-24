/**
 * @file NvsManager.cpp
 * @brief Implementation file for the NvsManager class.
 *
 * The NvsManager class provides functionality for initializing and accessing the Non-Volatile Storage (NVS) namespace.
 * It allows retrieving and setting key-value pairs stored in the NVS, with support for various data types.
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

template <>
void NvsManager::SetVar<uint8>(const char* key, uint8* value)
{
    esp_err_t err = nvs_set_u8(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint16>(const char* key, uint16* value)
{
    esp_err_t err = nvs_set_u16(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint32>(const char* key, uint32* value)
{
    esp_err_t err = nvs_set_u32(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint64>(const char* key, uint64* value)
{
    esp_err_t err = nvs_set_u64(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint8>(const char* key, sint8* value)
{
    esp_err_t err = nvs_set_i8(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint16>(const char* key, sint16* value)
{
    esp_err_t err = nvs_set_i16(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint32>(const char* key, sint32* value)
{
    esp_err_t err = nvs_set_i32(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint64>(const char* key, sint64* value)
{
    esp_err_t err = nvs_set_i64(my_handle, key, *value);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<std::string>(const char* key, std::string* value)
{
    esp_err_t err = nvs_set_str(my_handle, key, value->c_str());
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        logManager.Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
uint8 NvsManager::GetVar<uint8>(const char* key, uint8 defaultValue)
{
    uint8 value;
    esp_err_t err = nvs_get_u8(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<uint8>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
uint16 NvsManager::GetVar<uint16>(const char* key, uint16 defaultValue)
{
    uint16 value;
    esp_err_t err = nvs_get_u16(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<uint16>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
uint32 NvsManager::GetVar<uint32>(const char* key, uint32 defaultValue)
{
    uint32 value;
    esp_err_t err = nvs_get_u32(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<uint32>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
uint64 NvsManager::GetVar<uint64>(const char* key, uint64 defaultValue)
{
    uint64 value;
    esp_err_t err = nvs_get_u64(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<uint64>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
sint8 NvsManager::GetVar<sint8>(const char* key, sint8 defaultValue)
{
    sint8 value;
    esp_err_t err = nvs_get_i8(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<sint8>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
sint16 NvsManager::GetVar<sint16>(const char* key, sint16 defaultValue)
{
    sint16 value;
    esp_err_t err = nvs_get_i16(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<sint16>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
sint32 NvsManager::GetVar<sint32>(const char* key, sint32 defaultValue)
{
    sint32 value;
    esp_err_t err = nvs_get_i32(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<sint32>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
sint64 NvsManager::GetVar<sint64>(const char* key, sint64 defaultValue)
{
    sint64 value;
    esp_err_t err = nvs_get_i64(my_handle, key, &value);
    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<sint64>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS.");
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}

template <>
std::string NvsManager::GetVar<std::string>(const char* key, std::string defaultValue)
{
    std::string value;
    size_t required_size;
    esp_err_t err = nvs_get_str(my_handle, key, NULL, &required_size);

    if(ESP_OK == err)
    {
        char* char_str = (char*)malloc(required_size);
        if(char_str == NULL)
        {
            err = ESP_ERR_NO_MEM;
        }
        else
        {
            err = nvs_get_str(my_handle, key, char_str, &required_size);
            if(ESP_OK == err)
            {
                value = std::string(char_str);
            }
            free(char_str);
        }
    }

    if(ESP_OK != err)
    {
        if(ESP_ERR_NVS_NOT_FOUND == err)
        {
            value = defaultValue;
            SetVar<std::string>(key, &defaultValue);
        }
        else
        {
            logManager.Log(E, "NvsManager", "Error getting value from NVS: %s", esp_err_to_name(err));
            throw std::runtime_error("Error getting value from NVS.");
        }
    }
    return value;
}
