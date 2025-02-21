cmake_minimum_required(VERSION 3.22.1)

# set(CMAKE_TOOLCHAIN_FILE "toolchain-xmc.cmake")
project(xmc_ota LANGUAGES C ASM)

set(XMCLIB ${PROJECT_SOURCE_DIR}/deps/firment/firmware/port/XMC4/mtb-xmclib-cat3/XMCLib)

# Note: WolfBoot modifies CMAKE_C_COMPILER etc, in a file called 
# toolchain_arm-none-eabi.cmake  but this isn't the same as specifying a
# toolchain file for the whole project. 

include(wolfBootConfig.cmake)

add_subdirectory(deps/wolfBoot)

message(STATUS "IMAGE_HEADER_SIZE: ${IMAGE_HEADER_SIZE}")
math(EXPR APP_CODE_ADDRESS_CACHED "${CACHED_PARTITION_BOOT_ADDRESS} + ${IMAGE_HEADER_SIZE}"
     OUTPUT_FORMAT HEXADECIMAL)
math(EXPR APP_CODE_ADDRESS_DIRECT "${WOLFBOOT_PARTITION_BOOT_ADDRESS} + ${IMAGE_HEADER_SIZE}"
     OUTPUT_FORMAT HEXADECIMAL)
math(EXPR APP_CODE_SIZE "${WOLFBOOT_PARTITION_SIZE} - ${IMAGE_HEADER_SIZE}"
     OUTPUT_FORMAT HEXADECIMAL)
set(LINKER_FILE ${PROJECT_BINARY_DIR}/app.ld)
configure_file(firmware/XMC4700x2048.ld ${LINKER_FILE})

set(FIRMENT_DIR deps/firment)


if (1)
# TODO: this could move into fmt's CML.
add_subdirectory(${FIRMENT_DIR}/firmware/port/${FMT_PORT} ${FMT_PORT})

add_subdirectory(${FIRMENT_DIR} firment)

add_executable(app.elf
  firmware/main.c
  firmware/project_comms.c
  firmware/gpio_common.c
)
target_include_directories(app.elf PRIVATE
  firmware
)
target_link_libraries(app.elf FirmentFW MCUPort)
endif()

# Build a binary.
add_custom_target(app.bin ALL DEPENDS app.elf)
add_custom_command(TARGET app.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary app.elf app.bin)

# Sign the binary.
add_custom_target(signing ALL DEPENDS
  keystore
  ${PROJECT_BINARY_DIR}/app.bin
)
add_custom_command(TARGET signing
  COMMAND ${SIGN_TOOL} --ed25519 --sha256 
    ${PROJECT_BINARY_DIR}/app.bin 
    ${WOLFBOOT_SIGNING_PRIVATE_KEY}
    1
  COMMENT "Signing app.bin, creating app_v<x>_signed.bin"  
)