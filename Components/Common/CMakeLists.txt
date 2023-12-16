idf_component_register(SRCS
                        "Common.cpp"
                       INCLUDE_DIRS
                        "."
                       REQUIRES pthread
                       esp_wifi
                       LogManager
                       )