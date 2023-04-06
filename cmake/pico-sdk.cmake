# cmake-format: off
set(PICO_BOARD pico_w)

if(NOT DEFINED PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message(STATUS "Using PICO_SDK_PATH from the environment: " ${PICO_SDK_PATH})
    else()
        message(FATAL_ERROR "PICO_SDK_PATH is not defined")
        return()
    endif()
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

macro(initialize_pico)
    pico_sdk_init()
    
    if (NOT PICO_CYW43_SUPPORTED)
        message(FATAL_ERROR "PICO W is not set as the target or not supported by the SDK")
        return()
    endif()
endmacro()
# cmake-format: on
