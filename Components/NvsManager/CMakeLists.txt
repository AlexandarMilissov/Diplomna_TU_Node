idf_component_register(SRCS "NvsManager.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       nvs_flash
                       )