idf_component_register(SRCS "Node.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                       LogManager
                       TaskManager
                       NvsManager
                       WifiManager
                       EspnowDriver
                       EspnowManager
                       EspmeshDriver
                       EspmeshManager
                       EspmeshServer

                       Interfaces
                       )