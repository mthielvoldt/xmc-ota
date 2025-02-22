## What WolfBoot does

hal_init(): you defined this
wolfBoot_start() -> update_flash.c

### update_flash.c
call get_partition_state() on BOOT, UPDATE partitions. 
- Checks for magic # "BOOT" in specified partition. !!! 
  - !! FAILS in boot partition, Reads 0xFFFFFFFF, not "BOOT"
  - !! FAILS in update partition, reads 0, not "BOOT". 

- swap_and_final_erase(resume=1) in case power failed during swap. 
  - opens all 3 partitions.


## Macros Commonly used
PART_BOOT 0   PART_UPDATE 1   PART_NONE 0xFF

WOLFBOOT_MAGIC          0x464C4F57 /* WOLF */
WOLFBOOT_MAGIC_TRAIL    0x544F4F42 /* BOOT */

PART_BOOT_ENDFLAGS (WOLFBOOT_PARTITION_BOOT_ADDRESS + ENCRYPT_TMP_SECRET_OFFSET)

WOLFBOOT_SECTOR_SIZE 0x20000 !! WRONG !! should be 0x40000