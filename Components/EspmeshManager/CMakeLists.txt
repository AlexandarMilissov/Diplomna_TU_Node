idf_component_register(SRCS "EspmeshManager.cpp"
                       INCLUDE_DIRS
                       "."
                       "Messages"
                       REQUIRES
                       Common
                       EspnowManager
                       )