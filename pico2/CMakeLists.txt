cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico2 CACHE STRING "Pico board type")

include(pico_sdk_import.cmake)

project(furi_encryption C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_NO_PICOTOOL 0)

pico_sdk_init()

add_executable(furi_encryption
    src/main.cpp
    include/tiny-AES-c/aes.c
    include/cryptopp/allocate.cpp
    include/cryptopp/cpu.cpp
    include/cryptopp/cryptlib.cpp
    include/cryptopp/fips140.cpp
    include/cryptopp/filters.cpp
    include/cryptopp/hight.cpp
    include/cryptopp/lea.cpp
    include/cryptopp/misc.cpp
    include/cryptopp/rc5.cpp
    include/cryptopp/seed.cpp
    include/cryptopp/simeck.cpp
    include/cryptopp/simon.cpp
    include/cryptopp/speck.cpp
    include/cryptopp/tea.cpp
    include/cryptopp/twofish.cpp
    include/cryptopp/tftables.cpp
    
    src/algorithms/tinyaes_exp.cpp
    src/algorithms/lea_exp.cpp
    src/algorithms/tea_exp.cpp
    src/algorithms/rc5_exp.cpp
    src/algorithms/twofish_exp.cpp
    src/algorithms/simeck_exp.cpp
    src/algorithms/simon_exp.cpp
    src/algorithms/speck_exp.cpp
    src/algorithms/hight_exp.cpp
    src/algorithms/seed_exp.cpp
)

set_source_files_properties(
    include/cryptopp/allocate.cpp
    include/cryptopp/cpu.cpp
    include/cryptopp/cryptlib.cpp
    include/cryptopp/fips140.cpp
    include/cryptopp/filters.cpp
    include/cryptopp/hight.cpp
    include/cryptopp/lea.cpp
    include/cryptopp/misc.cpp
    include/cryptopp/rc5.cpp
    include/cryptopp/seed.cpp
    include/cryptopp/simeck.cpp
    include/cryptopp/simon.cpp
    include/cryptopp/speck.cpp
    include/cryptopp/tea.cpp
    include/cryptopp/twofish.cpp
    include/cryptopp/tftables.cpp
    
    src/algorithms/lea_exp.cpp
    src/algorithms/tea_exp.cpp
    src/algorithms/rc5_exp.cpp
    src/algorithms/twofish_exp.cpp
    src/algorithms/simeck_exp.cpp
    src/algorithms/simon_exp.cpp
    src/algorithms/speck_exp.cpp
    src/algorithms/hight_exp.cpp
    src/algorithms/seed_exp.cpp
    PROPERTIES
    COMPILE_OPTIONS "-fexceptions;-frtti"
)

target_compile_definitions(furi_encryption PRIVATE
    CRYPTOPP_DISABLE_ASM=1
    CRYPTOPP_DISABLE_LEA_SIMD=1
)

target_include_directories(furi_encryption PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/tiny-AES-c
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(furi_encryption
    pico_stdlib
)

pico_enable_stdio_usb(furi_encryption 1)
pico_enable_stdio_uart(furi_encryption 1)

pico_add_extra_outputs(furi_encryption)
