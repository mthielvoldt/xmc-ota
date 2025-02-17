cmake_minimum_required(VERSION 3.19)

# set(CMAKE_TOOLCHAIN_FILE "toolchain-xmc.cmake")
project(xmc_ota LANGUAGES C)

include(wolfBootConfig.cmake)

# Don't build the signing tool (we're cross compiling, that's a desktop tool.)
# set(PYTHON_KEYTOOLS yes)

set(XMCLIB ${PROJECT_SOURCE_DIR}/deps/mtb-xmclib-cat3/XMCLib)
set(USER_HAL_SOURCES
  ${XMCLIB}/src/xmc4_flash.c
)
set(USER_HAL_INCLUDE_DIRS
  ${XMCLIB}/inc
  ${PROJECT_SOURCE_DIR}/deps/mtb-xmclib-cat3/CMSIS/Infineon/COMPONENT_XMC4700/Include
  ${PROJECT_SOURCE_DIR}/deps/firment/firmware/lib/CMSIS-6.1.0/Core/Include
)
set(BUILD_TEST_APPS no)
set(BUILD_IMAGE yes)

add_subdirectory(deps/wolfBoot)

# add_executable(combined_image)

# target_link_libraries(combined_image
#   wolfboot)
