
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(PROJECT_CONFIG_DIR ${PROJECT_SOURCE_DIR}/firmware/config)

# These needed if you're using Firmware Update fmt_update.c

# XMC_FLASH_SECTOR_(0-8): Bootloader, 256kB total.
# XMC_FLASH_SECTOR_9:     Active App, 256kB
# XMC_FLASH_SECTOR_10:    App download, 256kB  <-- This file only uses this.
# XMC_FLASH_SECTOR_11:    Swap space, 256kB
set(FMT_IMAGE_DOWNLOAD_ADDRESS  ${WOLFBOOT_PARTITION_UPDATE_ADDRESS})
set(FMT_IMAGE_DOWNLOAD_SECTOR_SIZE ${WOLFBOOT_SECTOR_SIZE})