cmake_minimum_required(VERSION 3.0)

project(demo)

add_executable(${PROJECT_NAME} demo.c)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/radio.pio)

pico_set_program_name(${PROJECT_NAME} "pico-st7735")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# add libraries
target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib)
target_link_libraries(${PROJECT_NAME} PUBLIC lib-st7735)
target_link_libraries(${PROJECT_NAME} PUBLIC hardware_spi)
target_link_libraries(${PROJECT_NAME} PUBLIC hardware_pio)

pico_add_extra_outputs(${PROJECT_NAME})

# target_compile_definitions(
#   lib-st7735
#   PUBLIC
#   PIN_TFT_RST=${SPI_TFT_RST}
#   PIN_TFT_DC=${SPI_TFT_DC}
#   PIN_TFT_CS=${SPI_TFT_CS}
#   )


# use custom linker script
pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)
