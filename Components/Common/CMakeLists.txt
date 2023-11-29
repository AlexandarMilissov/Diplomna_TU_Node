idf_component_register(SRCS
                        "Common.c"
                       INCLUDE_DIRS
                        "."
                       REQUIRES pthread
                       esp_wifi
                       LogWrapper
                       )