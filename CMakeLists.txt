cmake_minimum_required(VERSION 3.22.1)

# set(CMAKE_TOOLCHAIN_FILE "toolchain-xmc.cmake")
project(xmc_ota LANGUAGES C ASM)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(XMCLIB ${PROJECT_SOURCE_DIR}/deps/firment/firmware/port/XMC4/mtb-xmclib-cat3/XMCLib)

# Note: WolfBoot modifies CMAKE_C_COMPILER etc, in a file called 
# toolchain_arm-none-eabi.cmake  but this isn't the same as specifying a
# toolchain file for the whole project. 

include(wolfBootConfig.cmake)
include(firmentConfig.cmake) # include after wolfBootConfig.cmake

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
target_link_options(app.elf PRIVATE "-T${LINKER_FILE}")

target_link_libraries(app.elf FirmentFW MCUPort wolfboot)

### Standalone app for debug. # TODO: DRY this with an add_library() layer. 
set(STANDALONE_LINKER_FILE 
  "${PROJECT_SOURCE_DIR}/deps/firment/firmware/port/XMC4/XMC4700x2048.ld")
add_executable(app_standalone.elf
  firmware/main.c
  firmware/project_comms.c
  firmware/gpio_common.c
)
target_include_directories(app_standalone.elf PRIVATE
  firmware
)
target_link_options(app_standalone.elf PRIVATE "-T${STANDALONE_LINKER_FILE}")
target_link_libraries(app_standalone.elf FirmentFW MCUPort wolfboot)

endif()

# Build firmware artifacts: signed app binary and combined [boot/app] binary.
add_custom_command(DEPENDS app.elf
  OUTPUT app.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary app.elf app.bin
)

# TODO: Refactor below artifacts creation to use wolfboot.cmake function:
# gen_wolfboot_factory_image()

add_custom_target(signed_app ALL DEPENDS app.bin keystore)
# Sign the binary.
add_custom_command(TARGET signed_app
  COMMAND IMAGE_HEADER_SIZE=${IMAGE_HEADER_SIZE} ${SIGN_TOOL} --ed25519 --sha256 
    ${PROJECT_BINARY_DIR}/app.bin 
    ${WOLFBOOT_SIGNING_PRIVATE_KEY}
    1
  COMMENT "Signing app.bin, creating app_v<x>_signed.bin"  
)

add_custom_target(factory_bin ALL DEPENDS 
  signed_app
  wolfboot_xmc4700_bin
  binAssemble
)
message("binAssemble CMD: ${BINASSEMBLE} factory_v1.bin ${ARCH_FLASH_OFFSET} wolfboot_xmc4700.bin ${BOOT_ADDRESS} app_v1_signed.bin")

add_custom_command(TARGET factory_bin
  COMMAND ${BINASSEMBLE} 
    factory_v1.bin 
    ${ARCH_FLASH_OFFSET}
    ${PROJECT_BINARY_DIR}/deps/wolfBoot/test-app/wolfboot_xmc4700.bin
    ${WOLFBOOT_PARTITION_BOOT_ADDRESS}
    ${PROJECT_BINARY_DIR}/app_v1_signed.bin
)