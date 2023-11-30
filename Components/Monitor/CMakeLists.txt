idf_component_register(SRCS "Monitor.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       EspnowStack
                       TaskManager
                       )