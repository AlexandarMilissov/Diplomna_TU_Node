idf_component_register(SRCS
                        "Monitor.cpp"
                        "CpuMonitor.cpp"
                        "RamMonitor.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       Common
                       EspnowManager
                       TaskManager
                       BaseComponent
                       )