cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(furi_encryption C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_NO_PICOTOOL 0)

pico_sdk_init()

add_executable(furi_encryption
    src/main.cpp
    include/tiny-AES-c/aes.c
)

target_include_directories(furi_encryption PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/tiny-AES-c
)

target_link_libraries(furi_encryption
    pico_stdlib
)

pico_enable_stdio_usb(furi_encryption 0)
pico_enable_stdio_uart(furi_encryption 1)

pico_add_extra_outputs(furi_encryption)
