idf_component_register(SRCS "TaskManager.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       Common
                       esp_timer
                       nvs_flash
                       EspnowStack
                       WifiManager
                       )