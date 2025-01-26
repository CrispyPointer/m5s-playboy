idf_component_register(SRCS "m5s_button.c"
                    INCLUDE_DIRS "include" 
                    PRIV_REQUIRES tiny_timer
                    REQUIRES defines driver)
