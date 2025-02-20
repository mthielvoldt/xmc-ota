cmake_minimum_required(VERSION 3.22.1)

# set(CMAKE_TOOLCHAIN_FILE "toolchain-xmc.cmake")
project(xmc_ota LANGUAGES C ASM)

set(XMCLIB ${PROJECT_SOURCE_DIR}/deps/firment/firmware/port/XMC4/mtb-xmclib-cat3/XMCLib)

# Note: WolfBoot modifies CMAKE_C_COMPILER etc, in a file called 
# toolchain_arm-none-eabi.cmake  but this isn't the same as specifying a
# toolchain file for the whole project. 

include(wolfBootConfig.cmake)

add_subdirectory(deps/wolfBoot)

set(FIRMENT_DIR deps/firment)

# TODO: this could move into fmt's CML.

if (1)
add_subdirectory(${FIRMENT_DIR}/firmware/port/${FMT_PORT} ${FMT_PORT})

add_subdirectory(${FIRMENT_DIR} firment)

add_executable(app
  firmware/main.c
  firmware/project_comms.c
  firmware/gpio_common.c
)
target_include_directories(app PRIVATE
  firmware
)
target_link_libraries(app FirmentFW MCUPort)
endif()

# target_link_libraries(combined_image
#   wolfboot)
