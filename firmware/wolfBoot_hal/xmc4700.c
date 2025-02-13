/** xmc4700.c
 * 
 */

#include <stdint.h>
#include <xmc4_flash.h>

#include <image.h>


#ifndef ARCH_FLASH_OFFSET
// Access address that bypasses the prefetch cache. Table 7-2
#define ARCH_FLASH_OFFSET XMC_FLASH_UNCACHED_BASE
#endif

/* Assembly helpers - Data Memory Barrier */
// #define DMB() asm volatile ("dmb")

/*** FLASH ***/
#define PAGE_SIZE_BYTES     256U

#define FLASH_TOP      (uint32_t *)(XMC_FLASH_UNCACHED_BASE + 0x0200000UL)

#define FLASH_SECTOR_COUNT 16
const uint32_t sector_base[FLASH_SECTOR_COUNT + 1] = {
    XMC_FLASH_SECTOR_0,  /* 16 Kb   */
    XMC_FLASH_SECTOR_1,  /* 16 Kb   */
    XMC_FLASH_SECTOR_2,  /* 16 Kb   */
    XMC_FLASH_SECTOR_3,  /* 16 Kb   */

    XMC_FLASH_SECTOR_4,  /* 16 Kb   */
    XMC_FLASH_SECTOR_5,  /* 16 Kb   */
    XMC_FLASH_SECTOR_6,  /* 16 Kb   */
    XMC_FLASH_SECTOR_7,  /* 16 Kb   */

    XMC_FLASH_SECTOR_8,  /* 128 Kb  */
    XMC_FLASH_SECTOR_9,  /* 256 Kb  */
    XMC_FLASH_SECTOR_10, /* 256 Kb  */
    XMC_FLASH_SECTOR_11, /* 256 Kb  */

    XMC_FLASH_SECTOR_12, /* 256 Kb  */
    XMC_FLASH_SECTOR_13, /* 256 Kb  */
    XMC_FLASH_SECTOR_14, /* 256 Kb  */
    XMC_FLASH_SECTOR_15, /* 256 Kb  */
    FLASH_TOP
};

void hal_init(void)
{
  // Max the system clock so crypto ops proceed quickly.
}

void hal_prepare_boot(void)
{
  // Put sysclock back to normal system startup defaults.
}

void RAMFUNCTION hal_flash_unlock(void)
{
  // There is no global flash unlock on XMC.
  // Each page write and sector erase operation requires a command sequence.

  // If write protection is enabled, it is temporarily disabled sector-by-
  // sector with a pre-stored user password.
}

void RAMFUNCTION hal_flash_lock(void) { }

/** hal_flash_write
 * @param address Either an offset from the program memory base address, or an
 * absolute address.  Un-aligned addresses are permitted.
 * @param data the payload to be stored in the flash.
 * @param len the size of the payload. Valid range: [1, WOLFBOOT_SECTOR_SIZE]
 * @returns 0 on success, a negative value on failure.
 * The XMC flash memory model only supports writing aligned 256-byte pages at a
 * time.  So this function implements a read-modify-write process than handles
 * both un-aligned start and end addresses.
 */
int RAMFUNCTION hal_flash_write(uint32_t address, const uint8_t *data, int len)
{
  uint8_t page_buffer[PAGE_SIZE_BYTES];
  int bytesWritten = 0;
  
  
  /* adjust for flash base to allow for both offsets and absolute addresses. */
  if (address < ARCH_FLASH_OFFSET) {
    address += ARCH_FLASH_OFFSET;
  }
  
  /* Find the closest page-aligned address preceeding first address to write*/
  uint32_t page_adr = getPreceedingPageBoundary(address);
  int last_address_to_write = address + len;
  uint32_t write_end_adr, write_start_adr, bytes_written = 0;

  while (last_address_to_write > page_adr) {
    memset(page_buffer, 0, PAGE_SIZE_BYTES);
    write_start_adr = address + bytes_written;
    write_end_adr = (page_adr + PAGE_SIZE_BYTES < last_address_to_write) ? 
      page_adr + PAGE_SIZE_BYTES : last_address_to_write;
    
    //  WRITE FIRST PAGE
    //      _______buffer______ 
    //     |.......+===========|=======+............|
    //     ^       ^           ^
    //  page_adr  wr_start   wr_end
    //
    //  WRITE SECOND PAGE
    //                          _______buffer_______
    //     |.......+===========|=======+............|
    //                         ^       ^
    //                     page_adr  wr_end
    //                     wr_start
    memcpy(page_buffer + (write_start_adr - page_adr),
           data + bytes_written,
           write_end_adr - write_start_adr
    );
    XMC_FLASH_ProgramPage((uint32_t*)page_adr, page_buffer);

    // Prepare for next page.
    bytes_written += write_end_adr - write_start_adr;
    page_adr += PAGE_SIZE_BYTES;
  }
  return 0;
}

/** hal_flash_erase
 * Erase all the sectors within the specified range, accounting for flash sector
 * geometries.
 * @param start_address marks the start of the area to erase. start_address is
 * guaranteed to be aligned to WOLFBOOT_SECTOR_SIZE.
 * @param len specifies the size of the area to be erased. Len is guaranteed to 
 * be a multiple of WOLFBOOT_SECTOR_SIZE. 
 */
int RAMFUNCTION hal_flash_erase(uint32_t start_address, int len)
{
  // Work with absolute addresses. 
  if (start_address < ARCH_FLASH_OFFSET) {
    start_address += ARCH_FLASH_OFFSET;
  }
  if (len == 0) {
    return -1;
  }
  
  uint32_t end_address = start_address + len - 1; 

  int start_sector = getSectorContainingAddress(start_address);
  int end_sector = getSectorContainingAddress(end_address);
  if (start_sector < 0 || end_sector < 0){
    return -1;
  }

  for (int sector = start_sector; sector <= end_sector; sector++)
    XMC_FLASH_EraseSector(sector_base[sector])
  return 0;
}

/**
 * @param address must be an absolute address (not an offset).
 */
static int getSectorContainingAddress(uint32_t address) {
  int ret = -1;
  for (int sectorIdx = 0; sectorIdx < FLASH_SECTOR_COUNT; sectorIdx++) {
    if ((address >= sector_base[sectorIdx]) && \
        (address <  sector_base[sectorIdx + 1])) {
          ret = sectorIdx;
    }
  }
  return ret;
}
