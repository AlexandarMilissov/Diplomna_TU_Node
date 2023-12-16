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

nvs_handle_t NvsManager::my_handle;

void NvsManager::Init(const void *pvParameters)
{
    LogManager::SetMinimalLevel("NvsManager", W);
    DUMMY_STATEMENT(pvParameters);

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

    nvs_open("Info", NVS_READWRITE, &my_handle);
}

template <>
uint8 NvsManager::GetVar<uint8>(const char* key)
{
    uint8 value;
    esp_err_t err = nvs_get_u8(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
uint16 NvsManager::GetVar<uint16>(const char* key)
{
    uint16 value;
    esp_err_t err = nvs_get_u16(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
uint32 NvsManager::GetVar<uint32>(const char* key)
{
    uint32 value;
    esp_err_t err = nvs_get_u32(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
uint64 NvsManager::GetVar<uint64>(const char* key)
{
    uint64 value;
    esp_err_t err = nvs_get_u64(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
sint8 NvsManager::GetVar<sint8>(const char* key)
{
    sint8 value;
    esp_err_t err = nvs_get_i8(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
sint16 NvsManager::GetVar<sint16>(const char* key)
{
    sint16 value;
    esp_err_t err = nvs_get_i16(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
sint32 NvsManager::GetVar<sint32>(const char* key)
{
    sint32 value;
    esp_err_t err = nvs_get_i32(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
sint64 NvsManager::GetVar<sint64>(const char* key)
{
    sint64 value;
    esp_err_t err = nvs_get_i64(my_handle, key, &value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return value;
}

template <>
std::string NvsManager::GetVar<std::string>(const char* key)
{
    size_t required_size;
    esp_err_t err = nvs_get_str(my_handle, key, NULL, &required_size);
    if(err == ESP_OK)
    {
        char* char_str = (char*)malloc(required_size);
        if(char_str == NULL)
        {
            err = ESP_ERR_NO_MEM;
        }
        else
        {
            err = nvs_get_str(my_handle, key, char_str, &required_size);
            if(err == ESP_OK)
            {
                return std::string(char_str);
            }
            free(char_str);
        }
    }

    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error getting value from NVS.");
        throw std::runtime_error("Error getting value from NVS.");
    }
    return std::string();
}

template <>
void NvsManager::SetVar<uint8>(const char* key, uint8* value)
{
    esp_err_t err = nvs_set_u8(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint16>(const char* key, uint16* value)
{
    esp_err_t err = nvs_set_u16(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint32>(const char* key, uint32* value)
{
    esp_err_t err = nvs_set_u32(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<uint64>(const char* key, uint64* value)
{
    esp_err_t err = nvs_set_u64(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint8>(const char* key, sint8* value)
{
    esp_err_t err = nvs_set_i8(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint16>(const char* key, sint16* value)
{
    esp_err_t err = nvs_set_i16(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint32>(const char* key, sint32* value)
{
    esp_err_t err = nvs_set_i32(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<sint64>(const char* key, sint64* value)
{
    esp_err_t err = nvs_set_i64(my_handle, key, *value);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}

template <>
void NvsManager::SetVar<std::string>(const char* key, std::string* value)
{
    esp_err_t err = nvs_set_str(my_handle, key, value->c_str());
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error setting value to NVS.");
        throw std::runtime_error("Error setting value to NVS.");
    }
    err = nvs_commit(my_handle);
    if(ESP_OK != err)
    {
        LogManager::Log(E, "NvsManager", "Error committing value to NVS.");
        throw std::runtime_error("Error committing value to NVS.");
    }
}
