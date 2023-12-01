#include "Common.h"
#include "NvsManager.h"
#include "nvs_flash.h"

nvs_handle_t my_handle;
size_t length;
char* saved_name = NULL;
#define NVS_GET_STR_SIZE(handle, key, out_length) (nvs_get_str(handle, key, NULL, out_length))

void ReadName();

void NvsManager_Init(const void *pvParameters)
{
    LogManager_SetMinimalLevel("NvsManager", W);
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

    ReadName();
}

void ReadName()
{
    esp_err_t err;
    nvs_open("Info", NVS_READWRITE, &my_handle);

    NVS_GET_STR_SIZE(my_handle, "name", &length);
    saved_name = malloc(sizeof(char) * length);
    err = nvs_get_str(my_handle, "name", saved_name, &length);
    if(ESP_ERR_NVS_NOT_FOUND == err)
    {
        nvs_set_str(my_handle, "name", "Default");
        nvs_commit(my_handle);
        LogManager_Log(W, "NvsManager", "Setting name.\n");
    }

    nvs_close(my_handle);
}

const char* NvsGetName()
{
    return (const char*)(saved_name);
}
