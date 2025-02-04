idf_component_register(SRCS "main.cpp"
                    PRIV_REQUIRES spi_flash m5s_button tiny_timer display tetris
                    REQUIRES defines app
                    INCLUDE_DIRS "")
