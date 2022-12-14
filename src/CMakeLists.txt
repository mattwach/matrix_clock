cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )


add_executable(matrix_clock
        buttons.c
        clock.c
        clock_render.c
        clock_settings.c
        colors.c
        debounce.c
        led_matrix_dotstar.c
        main.c
        number_draw.c
        set_time.c
        render/matrix.c
        )

# pull in common dependencies
target_link_libraries(matrix_clock
    pico_stdlib
    UART_CONSOLE
    hardware_spi
    hardware_i2c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(matrix_clock 1)
pico_enable_stdio_uart(matrix_clock 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(matrix_clock)

add_subdirectory(pico_uart_console)
