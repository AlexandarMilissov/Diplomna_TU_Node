idf_component_register(SRCS "WifiDriver.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       esp_wifi
                       NetworkComponents
                       BaseComponent
                       )