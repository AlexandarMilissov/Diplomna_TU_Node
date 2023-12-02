idf_component_register(SRCS "Monitor.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       EspnowManager
                       TaskManager
                       )