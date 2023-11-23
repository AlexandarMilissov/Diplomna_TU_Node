idf_component_register(SRCS "NvsManager.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       Common
                       nvs_flash
                       )