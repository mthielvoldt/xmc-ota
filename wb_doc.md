# Major take-aways
## Wolfboot specifc
1. Wolfboot assumes plenty of erase sectors in each partition. 
  - What's plenty? 

## Flash-mem related
2. Flash can demand coddling.  Thin margins; state is fragile.
  - Programming can corrupt nearby cell data. 
  - Prefers 1 write per page policy
  - Erases disturb adjacent logical sectors

3. Flash is slow-ish (typical values)
  - Read: 6 clock cycles (min 22ns - default assumes running fast clock) 
  - Write/Erase: ~50kB/s: 
    - Write in 256B-pages, 5.5ms ea. Double-delay if weak bits detected
    - Erase in 16kB-256kB minimum chunks at .3 - 5.5s ea.


# What WolfBoot does

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

# WB API
## wolfBoot_update_trigger()

### hal_flash_erase(lastSector, SECTOR_FLAGS_SIZE)
  - lastSector = 0x0c08.0000 
  - SECTOR_FLAGS_SIZE = 3fffb (= 0x40000 - 5 [256kB - 5])
  This seems to erase the UPDATE_PARTITION I just set? 
  I can confirm, I have a header "WOLF..." written to 0c08.0000 before this.
  ... And, after the erase, all 0's. 
  **Take-away #1.  WB assumes erasing the last sector doesn't touch the FW**
hal_flash_erase()
  - start_sector = end_sector = 0xA (10) [Good]
hal_flash_write()
  - Cheked out, works. 

### set_partition_state(1, 0x70) -> set_trailer_at(1, 1, 0x70)
hal_flash_write()
**Lesson 2: writing state byte CORRUPTS neighboring data!  0x54 'T' -> 0x5c**



# Wolfboot issues
## wolfBoot_swap_and_final_erase()

Line 226 uintptr_t tmpBootPos = WOLFBOOT_PARTITION_SIZE - eraseLen -
        WOLFBOOT_SECTOR_SIZE;

If partition size == sector size, this becomes some negative #, represented as aptr (unsigned int) I was seeing its value == fffc0000. (-256k equivalent)

This is probably meant to be inside the partition still.  If PArtition had 10 sectors, this would be +8 sector sizes.

This location gets stored in tmpBuffer, (just 1 byte)
That gets compared with MAGIC_TRAIL, a match indicating swapDone. 

So were expecting "BOOT" at tmpBootPos? 