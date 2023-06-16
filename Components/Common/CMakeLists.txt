idf_component_register(SRCS 
                        "Common.c"
                        "Lists/Lists.c"
                       INCLUDE_DIRS 
                        "."
                        "./Lists"
                       REQUIRES pthread
                       )