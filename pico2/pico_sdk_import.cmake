if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/include/pico-sdk")
endif()

if (NOT EXISTS "${PICO_SDK_PATH}/pico_sdk_init.cmake")
    message(FATAL_ERROR "Pico SDK not found at ${PICO_SDK_PATH}")
endif()

include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
