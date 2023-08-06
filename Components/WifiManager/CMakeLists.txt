idf_component_register(SRCS "WifiManager.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       Common
                       esp_wifi
                       )