idf_component_register(SRCS "LogWrapper.c"
                        "Esp_LogWrapper/Esp_LogWrapper.c"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "./Esp_LogWrapper"
                       REQUIRES
                       Common
                       )