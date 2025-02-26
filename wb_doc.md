# Major take-aways
## Wolfboot specifc
1. Wolfboot assumes plenty of erase sectors in each partition. 
  - What's plenty? 
2. The swap sector considered to be one SECTOR_SIZE
3. Wolfboot defaults to erased state == 0xFF (1's).  Use WOLFBOOT_FLAGS_INVERT.




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
## wolfBoot_start (update_flash.c)
- check partition states on BOOT, UPDATE. 
- checks if an update was interrupted by reset, and resumes if so. 
- 

## wolfBoot_update
This can be called either before or AFTER completing wolfBoot_swap. 

## wolfBoot_swap_and_final_erase "finalize" would be better
This function is really all about managing the status bytes in update and boot.
Assuming no interrupt while in the last step (a covered scenario), there are two different cases.
1. The second-to-last sector (with status byte) contains image data. 
2. The second-to-last sector contains no image data (image is entirely earlier)

In both cases, the target partition states are the same: 
partition:  boot    swap    update
state:      TEST    any     NEW
TEST so a revert can happen if new image fails.  
NEW so WB knows not to try another update without a revert scenario. 

### Scenario 1.
finalize() is called with the following partition states: 
Partition:  boot     swap     update
state:      


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