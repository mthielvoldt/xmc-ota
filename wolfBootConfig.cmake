set(ARCH ARM)

set(WOLFBOOT_TARGET xmc4700 CACHE INTERNAL "Target platform")

set(SIGN ED25519)
set(HASH SHA256)
set(VTOR 1)
set(SPMATH 1)
set(WOLFBOOT_PARTITION_SIZE              0x40000) # 256kB
set(WOLFBOOT_SECTOR_SIZE                 0x40000)
set(WOLFBOOT_PARTITION_BOOT_ADDRESS   0x0C040000) # sector 9
set(CACHED_PARTITION_BOOT_ADDRESS     0x08040000)
set(WOLFBOOT_PARTITION_UPDATE_ADDRESS 0x0C080000) # sector 10
set(WOLFBOOT_PARTITION_SWAP_ADDRESS   0x0C0C0000) # sector 11

# XMC4 requires the VTOR be 1024-byte aligned. (256 4-byte words)
# see Vector Table Offset Register 2-64 (p123)
set(IMAGE_HEADER_SIZE 1024)

set(USER_HAL_SOURCES
  ${XMCLIB}/src/xmc4_flash.c
)
set(USER_HAL_INCLUDE_DIRS
  ${XMCLIB}/inc
  ${XMCLIB}/../CMSIS/Infineon/COMPONENT_XMC4700/Include
  ${PROJECT_SOURCE_DIR}/deps/firment/firmware/lib/CMSIS-6.1.0/Core/Include
)

set(KEYTOOLS_DIR ${PROJECT_BINARY_DIR}/../keytools)

set(BUILD_TEST_APPS no)
set(BUILD_IMAGE yes)