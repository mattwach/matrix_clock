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

# Choose a clock source
set(CLOCK_SOURCE clock_ds3231)
#set(CLOCK_SOURCE clock_pico_internal)

# Choose an led matrix
set(LED_MATRIX_SOURCE led_matrix_dotstar)
set(LED_MATRIX_WIDTH 8)
set(LED_MATRIX_HEIGHT 8)
set(DEFAULT_BRIGHTNESS_STEP 3)
#set(LED_MATRIX_SOURCE led_matrix_32x64)
#set(LED_MATRIX_WIDTH 32)
#set(LED_MATRIX_HEIGHT 64)
#set(DEFAULT_BRIGHTNESS_STEP 10)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        -DLED_MATRIX_WIDTH=${LED_MATRIX_WIDTH}
        -DLED_MATRIX_HEIGHT=${LED_MATRIX_HEIGHT}
        -DDEFAULT_BRIGHTNESS_STEP=${DEFAULT_BRIGHTNESS_STEP}
        )


add_executable(matrix_clock
        ${CLOCK_SOURCE}.c
        ${LED_MATRIX_SOURCE}.c
        buttons.c
        clock_render.c
        clock_settings.c
        colors.c
        debounce.c
        debug.c
        main.c
        number_draw.c
        set_time.c
        render/blank.c
        render/fade.c
        render/matrix.c
        render/number_cascade.c
        render/number_grid.c
        )

# pull in common dependencies
target_link_libraries(matrix_clock
    pico_stdlib
    UART_CONSOLE
)

if(CLOCK_SOURCE STREQUAL clock_pico_internal)
target_link_libraries(matrix_clock
    hardware_rtc
)
endif()

if(CLOCK_SOURCE STREQUAL clock_ds3231)
target_link_libraries(matrix_clock
    hardware_i2c
)
endif()

if(LED_MATRIX_SOURCE STREQUAL led_matrix_dotstar)
target_link_libraries(matrix_clock
    hardware_spi
)
endif()

if(LED_MATRIX_SOURCE STREQUAL led_matrix_32x64)
target_link_libraries(matrix_clock
    pico_multicore
)
endif()

# enable usb output, disable uart output
pico_enable_stdio_usb(matrix_clock 1)
pico_enable_stdio_uart(matrix_clock 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(matrix_clock)

add_subdirectory(pico_uart_console)
