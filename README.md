# An XMC4700 remote FW Update using wolfBoot

## Learnings

### About Wolfboot build
- WolfBoot is built with the expectation that you modify the code.  For example, if using your own HAL for an arm chip, and Cmake, you're exepected to:
  - write your .c and .ld files in the <wolfboot_root>/hal dir.
  - add your target name to the list of options in wolfBoot/CMakeLists.txt
  - modify test-app/startup_arm.c to provide your chip's vector table.

- Wolfboot uses the same startup file for all ARM chips 🤔. Not sure how this works: the startup file includes the vector table, which is definitely MCU specific. See wolfBoot/test-app/CMakeLists.
- WolfBoot seems to have a number of issues with cmake support.  Seems like cmake use is still quite unstable.  Examples on github use make. 
  - cmake build will fail at the `Building signing tool` step due to missing sources and #ifdef.  However, `make keytools` from the wolfboot root dir succeeds.

- "Gotcha": Using "make" will generate `target.h` in wolfBoot/src, whereas cmake generates `target.h` in the CMAKE_CURRENT_BINARY_DIR.  Make's copy will take precedence when the bootloader image is built.  So if you're using cmake, **Be sure to delete make's copy** This file is .gitignored, so git won't help make this obvious. 

### Wolfboot debug
- Backup (using old copy of FW if new one fails) is ENABLED by default.  The double negative, defaulting DISABLE_BACKUP option to "Disabled" (setting to "no") confuses.

#### Deps needed for WolfBoot usage
- needed `pip install wolfcrypt` for "Generating keystore.c and signing private key" build step. 
- libtool for needed to `pip install wolfcrypt`

### About Chip MFGs
- Flash program memory and the way you write it varies significantly chip to chip.
- Infineon uses an uncommon convention describing flash memory state: 
  Infineon: Programmed = "1"  Erased = "0".  
  Others: Programmed = "0", Erased = "1"

### About Hardware
- Some MFGs have cache mechanisms for memory access.  These only serve to accelerate flash read-access.  It'll have two base-addresses for the same physical flash.  The two base addresses are essentially parallel access paths to the same memory, with one going through the cache HW, the other bypassing it.  All writes (erase, program) to flash will use the cache-bypass (non-cached) address.
- A flash cell (bit) that is already in the programmed state (Infineon 1) can be written to with the erased-state (Infineon 0) over and over without disturbing the original state. 

### About Linker and Startup code
- Some MFGs use c for startup files (ST).  Others use .S files - assembly code that passes through the c preprocessor first.  (Infineon).
- If you're looking at assembly, and see a label you don't recognize, it might be defined below in the same file (assembly essentially works like "hoisting")