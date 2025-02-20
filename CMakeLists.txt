cmake_minimum_required(VERSION 3.19)

# set(CMAKE_TOOLCHAIN_FILE "toolchain-xmc.cmake")
project(xmc_ota LANGUAGES C)

set(XMCLIB ${PROJECT_SOURCE_DIR}/deps/mtb-xmclib-cat3/XMCLib)

set(MCU_VARIANT "XMC4700")
set(MCU_SPEED_VARIANT "F144")
set(MCU_MEM_VARIANT "2048")
# add_compile_definitions(XMC4400_F100x512)
add_compile_definitions(${MCU_VARIANT}_${MCU_SPEED_VARIANT}x${MCU_MEM_VARIANT})

include(wolfBootConfig.cmake)

add_subdirectory(deps/wolfBoot)

# add_executable(combined_image)

# target_link_libraries(combined_image
#   wolfboot)
