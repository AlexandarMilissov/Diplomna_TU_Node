idf_component_register(SRCS "WifiManager.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       esp_wifi
                       EspnowDriver
                       EspmeshDriver
                       )