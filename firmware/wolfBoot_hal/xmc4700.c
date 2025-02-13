/** xmc4700.c
 * 
 */

#include <stdint.h>

#include "image.h"


#ifndef ARCH_FLASH_OFFSET
// Cached mirror. Table 7-2
#define ARCH_FLASH_OFFSET 0x08000000U
#endif

/* Assembly helpers - Data Memory Barrier */
#define DMB() asm volatile ("dmb")


/*** FLASH ***/
#define APB1_CLOCK_ER           (*(volatile uint32_t *)(0x40023840U))
#define APB1_CLOCK_RST          (*(volatile uint32_t *)(0x40023820U))
#define TIM2_APB1_CLOCK_ER_VAL     (1 << 0)
#define PWR_APB1_CLOCK_ER_VAL   (1 << 28)

#define FLASH_BASE          (0x40023C00U)
#define FLASH_ACR           (*(volatile uint32_t *)(FLASH_BASE + 0x00U))
#define FLASH_KEYR          (*(volatile uint32_t *)(FLASH_BASE + 0x04U)) // unlock
#define FLASH_SR            (*(volatile uint32_t *)(FLASH_BASE + 0x0CU)) //
#define FLASH_CR            (*(volatile uint32_t *)(FLASH_BASE + 0x10U)) // all flash ops

/* Register values */
#define FLASH_ACR_RESET_DATA_CACHE            (1 << 12)
#define FLASH_ACR_RESET_INST_CACHE            (1 << 11)
#define FLASH_ACR_ENABLE_DATA_CACHE           (1 << 10)
#define FLASH_ACR_ENABLE_INST_CACHE           (1 << 9)
#define FLASH_ACR_ENABLE_PRFT                 (1 << 8)

#define FLASH_SR_BSY                          (1 << 16) // to wait for complete.
#define FLASH_SR_PGSERR                       (1 << 7) // used to clear errors. 
#define FLASH_SR_PGPERR                       (1 << 6)
#define FLASH_SR_PGAERR                       (1 << 5)
#define FLASH_SR_WRPERR                       (1 << 4)
#define FLASH_SR_OPERR                        (1 << 1)
#define FLASH_SR_EOP                          (1 << 0)

#define FLASH_CR_LOCK                         (1UL << 31) // unlock
#define FLASH_CR_ERRIE                        (1 << 25)
#define FLASH_CR_EOPIE                        (1 << 24)
#define FLASH_CR_STRT                         (1 << 16)
#define FLASH_CR_MER                          (1 << 2)
#define FLASH_CR_SER                          (1 << 1)
#define FLASH_CR_PG                           (1 << 0) // 

#define FLASH_CR_SNB_SHIFT                      3
#define FLASH_CR_SNB_MASK                      0x1f

#define FLASH_CR_PROGRAM_X8                   (0 << 8)
#define FLASH_CR_PROGRAM_X16                  (1 << 8)
#define FLASH_CR_PROGRAM_X32                  (2 << 8)
#define FLASH_CR_PROGRAM_X64                  (3 << 8)

#define FLASH_KEY1                            (0x45670123U) // unlock
#define FLASH_KEY2                            (0xCDEF89ABU) // unlock


/* FLASH Geometry */
#define FLASH_SECTOR_0  0x0000000 /* 16 Kb   */
#define FLASH_SECTOR_1  0x0004000 /* 16 Kb   */
#define FLASH_SECTOR_2  0x0008000 /* 16 Kb   */
#define FLASH_SECTOR_3  0x000C000 /* 16 Kb   */
#define FLASH_SECTOR_4  0x0010000 /* 64 Kb   */
#define FLASH_SECTOR_5  0x0020000 /* 128 Kb  */
#define FLASH_SECTOR_6  0x0040000 /* 128 Kb  */
#define FLASH_SECTOR_7  0x0060000 /* 128 Kb  */
#define FLASH_SECTOR_8  0x0080000 /* 128 Kb  */
#define FLASH_SECTOR_9  0x00A0000 /* 128 Kb  */
#define FLASH_SECTOR_10 0x00C0000 /* 128 Kb  */
#define FLASH_SECTOR_11 0x00E0000 /* 128 Kb  */
#define FLASH_TOP       0x0100000

#define FLASH_SECTORS 12
const uint32_t flash_sector[FLASH_SECTORS + 1] = {
    FLASH_SECTOR_0,
    FLASH_SECTOR_1,
    FLASH_SECTOR_2,
    FLASH_SECTOR_3,
    FLASH_SECTOR_4,
    FLASH_SECTOR_5,
    FLASH_SECTOR_6,
    FLASH_SECTOR_7,
    FLASH_SECTOR_8,
    FLASH_SECTOR_9,
    FLASH_SECTOR_10,
    FLASH_SECTOR_11,
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
    FLASH_CR |= FLASH_CR_LOCK;
    FLASH_KEYR = FLASH_KEY1;
    FLASH_KEYR = FLASH_KEY2;
}

void RAMFUNCTION hal_flash_lock(void)
{
    FLASH_CR |= FLASH_CR_LOCK;
}

int RAMFUNCTION hal_flash_write(uint32_t _address, const uint8_t *data, int len)
{
    int i;
    uint32_t address = _address - ARCH_FLASH_OFFSET;
    flash_wait_complete();
    clear_errors();
    /* Set 8-bit write */
    FLASH_CR &= (~(0x03 << 8));
    for (i = 0; i < len; i++) {
        FLASH_CR |= FLASH_CR_PG;
        *(uint8_t*)(address + (uint32_t)i) = data[i];
        flash_wait_complete();
        FLASH_CR &= ~FLASH_CR_PG;
    }
    return 0;
}

int RAMFUNCTION hal_flash_erase(uint32_t _address, int len)
{
    int start = -1, end = -1;
    uint32_t end_address;
    int i;
    uint32_t address = _address - ARCH_FLASH_OFFSET;
    if (len == 0)
        return -1;

    end_address = address + len - 1;

    if (address < flash_sector[0] || end_address > FLASH_TOP)
        return -1;
    for (i = 0; i < FLASH_SECTORS; i++)
    {
        if ((address >= flash_sector[i]) && (address < flash_sector[i + 1])) {
            start = i;
        }
        if ((end_address >= flash_sector[i]) && (end_address < flash_sector[i + 1])) {
            end = i;
        }
        if (start > 0 && end > 0)
            break;
    }
    if (start < 0 || end < 0)
        return -1;
    for (i = start; i <= end; i++)
        flash_erase_sector(i);
    return 0;
}
