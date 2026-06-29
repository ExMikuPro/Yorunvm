#ifndef YORUNVM_H
#define YORUNVM_H

/* =========================================================
 *  Auto-detect and include STM32 HAL header
 * =========================================================
 * Manual override example:
 *   #define YORUNVM_HAL_HEADER "stm32h7xx_hal.h"
 *   #include "yorunvm.h"
 */
#ifndef YORUNVM_HAL_HEADER
  #if defined(__has_include)
    #if __has_include("stm32h7xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32h7xx_hal.h"
    #elif __has_include("stm32f7xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32f7xx_hal.h"
    #elif __has_include("stm32f4xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32f4xx_hal.h"
    #elif __has_include("stm32g4xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32g4xx_hal.h"
    #elif __has_include("stm32g0xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32g0xx_hal.h"
    #elif __has_include("stm32l4xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32l4xx_hal.h"
    #elif __has_include("stm32l0xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32l0xx_hal.h"
    #elif __has_include("stm32f1xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32f1xx_hal.h"
    #elif __has_include("stm32c0xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32c0xx_hal.h"
    #elif __has_include("stm32u5xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32u5xx_hal.h"
    #elif __has_include("stm32wbxx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32wbxx_hal.h"
    #elif __has_include("stm32wlxx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32wlxx_hal.h"
    #elif __has_include("stm32xx_hal.h")
      #define YORUNVM_HAL_HEADER "stm32xx_hal.h"
    #endif
  #endif
#endif

#ifdef YORUNVM_HAL_HEADER
  #include YORUNVM_HAL_HEADER
#else
  #error "yorunvm.h: Please define YORUNVM_HAL_HEADER before including yorunvm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================
 *  User Configuration
 * ========================================================= */

/* Master switch: 1 = enabled, 0 = disabled */
#ifndef YORUNVM_ENABLE
  #define YORUNVM_ENABLE 1
#endif

/* Flash access switch: 1 = enable Flash APIs, 0 = remove them */
#ifndef YORUNVM_ENABLE_FLASH
  #define YORUNVM_ENABLE_FLASH 1
#endif

/* EEPROM access switch: 1 = enable EEPROM APIs, 0 = remove them */
#ifndef YORUNVM_ENABLE_EEPROM
  #define YORUNVM_ENABLE_EEPROM 0
#endif

#if !YORUNVM_ENABLE_FLASH && !YORUNVM_ENABLE_EEPROM
  #error "yorunvm.h: At least one of YORUNVM_ENABLE_FLASH or YORUNVM_ENABLE_EEPROM must be enabled"
#endif

#if YORUNVM_ENABLE_FLASH && YORUNVM_ENABLE_EEPROM
  #define YORUNVM_DUAL_REGION 1
#else
  #define YORUNVM_DUAL_REGION 0
#endif

/* Optional lock hooks, useful when NVM access is shared across contexts */
#ifndef YORUNVM_LOCK
  #define YORUNVM_LOCK()   do{}while(0)
#endif

#ifndef YORUNVM_UNLOCK
  #define YORUNVM_UNLOCK() do{}while(0)
#endif

/* Platform geometry defaults.
 * Users are encouraged to override these macros when targeting another STM32.
 */
#ifndef YORUNVM_FLASH_BASE
  #if defined(FLASH_BASE)
    #define YORUNVM_FLASH_BASE FLASH_BASE
  #else
    #define YORUNVM_FLASH_BASE 0x08000000u
  #endif
#endif

#ifndef YORUNVM_FLASH_SIZE_BYTES
  #if defined(STM32H743xx)
    #define YORUNVM_FLASH_SIZE_BYTES 0x00200000u
  #else
    #define YORUNVM_FLASH_SIZE_BYTES 0u
  #endif
#endif

#ifndef YORUNVM_FLASH_BANK_SIZE
  #if defined(STM32H743xx)
    #define YORUNVM_FLASH_BANK_SIZE 0x00100000u
  #else
    #define YORUNVM_FLASH_BANK_SIZE YORUNVM_FLASH_SIZE_BYTES
  #endif
#endif

#ifndef YORUNVM_FLASH_SECTOR_SIZE
  #if defined(STM32H743xx)
    #define YORUNVM_FLASH_SECTOR_SIZE 0x00020000u
  #else
    #define YORUNVM_FLASH_SECTOR_SIZE 0u
  #endif
#endif

#ifndef YORUNVM_FLASH_SECTORS_PER_BANK
  #if (YORUNVM_FLASH_BANK_SIZE != 0u) && (YORUNVM_FLASH_SECTOR_SIZE != 0u)
    #define YORUNVM_FLASH_SECTORS_PER_BANK (YORUNVM_FLASH_BANK_SIZE / YORUNVM_FLASH_SECTOR_SIZE)
  #else
    #define YORUNVM_FLASH_SECTORS_PER_BANK 0u
  #endif
#endif

#ifndef YORUNVM_FLASH_WRITE_UNIT
  #if defined(STM32H743xx)
    #define YORUNVM_FLASH_WRITE_UNIT 32u
  #else
    #define YORUNVM_FLASH_WRITE_UNIT 0u
  #endif
#endif

#ifndef YORUNVM_FLASH_VOLTAGE_RANGE
  #define YORUNVM_FLASH_VOLTAGE_RANGE FLASH_VOLTAGE_RANGE_3
#endif

#ifndef YORUNVM_EEPROM_BASE
  #define YORUNVM_EEPROM_BASE 0u
#endif

#ifndef YORUNVM_EEPROM_SIZE_BYTES
  #define YORUNVM_EEPROM_SIZE_BYTES 0u
#endif

/* Optional address-to-pointer override for host tests or custom mapping. */
#ifndef YORUNVM_ADDR_TO_PTR
  #define YORUNVM_ADDR_TO_PTR(addr) ((const uint8_t *)(unsigned long)(addr))
#endif

#ifndef YORUNVM_FLASH_PROGRAM_CALL
  #define YORUNVM_FLASH_PROGRAM_CALL(addr, src) \
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (addr), (uint32_t)(unsigned long)(src))
#endif

/* =========================================================
 *  Type Definitions
 * ========================================================= */

typedef enum
{
    YORUNVM_OK = 0,
    YORUNVM_ERROR = 1,
    YORUNVM_INVALID_PARAM = 2,
    YORUNVM_INVALID_ADDR = 3,
    YORUNVM_RANGE_ERROR = 4,
    YORUNVM_ALIGNMENT_ERROR = 5,
    YORUNVM_ERASE_ERROR = 6,
    YORUNVM_WRITE_ERROR = 7,
    YORUNVM_READ_ERROR = 8,
    YORUNVM_UNSUPPORTED = 9,
    YORUNVM_NOT_INITIALIZED = 10
} YORUNVM_StatusTypeDef;

typedef struct
{
    uint32_t BaseAddr;
    uint32_t EndAddr;
    uint8_t Initialized;
#if YORUNVM_DUAL_REGION
    uint8_t RegionType;
#endif
} YORUNVM_HandleTypeDef;

/* Global singleton storage.
 * Define YORUNVM_DEFINE_GLOBALS in exactly one .c file before including yorunvm.h.
 */
#if defined(YORUNVM_DEFINE_GLOBALS)
YORUNVM_HandleTypeDef hYorunvm;
#else
extern YORUNVM_HandleTypeDef hYorunvm;
#endif

/* =========================================================
 *  Internal Helpers
 * ========================================================= */

static inline unsigned yorunvm__region_contains_(uint32_t base, uint32_t end, uint32_t addr)
{
    return (unsigned)((addr >= base) && (addr < end));
}

static inline unsigned yorunvm__range_ok_(uint32_t base, uint32_t end, uint32_t addr, uint32_t len)
{
    uint32_t last;

    if (len == 0u) {
        return 1u;
    }

    if ((addr < base) || (addr >= end)) {
        return 0u;
    }

    last = addr + len - 1u;
    if (last < addr) {
        return 0u;
    }

    return (unsigned)(last < end);
}

static inline void yorunvm__copy_from_addr_(void *dst, uint32_t addr, uint32_t len)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = YORUNVM_ADDR_TO_PTR(addr);

    while (len-- != 0u) {
        *d++ = *s++;
    }
}

static inline unsigned yorunvm__detect_region_(uint32_t base, uint32_t *end_out, uint8_t *type_out)
{
#if YORUNVM_DUAL_REGION
#if YORUNVM_ENABLE_FLASH
    if ((YORUNVM_FLASH_SIZE_BYTES != 0u) &&
        yorunvm__region_contains_(YORUNVM_FLASH_BASE, YORUNVM_FLASH_BASE + YORUNVM_FLASH_SIZE_BYTES, base)) {
        *end_out = YORUNVM_FLASH_BASE + YORUNVM_FLASH_SIZE_BYTES;
        *type_out = 1u;
        return 1u;
    }
#endif

#if YORUNVM_ENABLE_EEPROM
    if ((YORUNVM_EEPROM_SIZE_BYTES != 0u) &&
        yorunvm__region_contains_(YORUNVM_EEPROM_BASE, YORUNVM_EEPROM_BASE + YORUNVM_EEPROM_SIZE_BYTES, base)) {
        *end_out = YORUNVM_EEPROM_BASE + YORUNVM_EEPROM_SIZE_BYTES;
        *type_out = 2u;
        return 1u;
    }
#endif

    *end_out = 0u;
    *type_out = 0u;
    return 0u;
#else
    (void)base;
    (void)end_out;
    (void)type_out;
    return 0u;
#endif
}

static inline YORUNVM_StatusTypeDef yorunvm__require_init_(uint8_t expected_region, uint32_t addr, uint32_t len)
{
    if (!hYorunvm.Initialized) {
        return YORUNVM_NOT_INITIALIZED;
    }

#if YORUNVM_DUAL_REGION
    if ((expected_region != 0u) && (hYorunvm.RegionType != expected_region)) {
        return YORUNVM_RANGE_ERROR;
    }
#else
    (void)expected_region;
#endif

    if (!yorunvm__range_ok_(hYorunvm.BaseAddr, hYorunvm.EndAddr, addr, len)) {
        return YORUNVM_RANGE_ERROR;
    }

    return YORUNVM_OK;
}

#if YORUNVM_ENABLE_FLASH
static inline YORUNVM_StatusTypeDef yorunvm__flash_erase_one_bank_(uint32_t bank, uint32_t start_sector, uint32_t count)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_error = 0u;

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = bank;
    erase.Sector = start_sector;
    erase.NbSectors = count;
    erase.VoltageRange = YORUNVM_FLASH_VOLTAGE_RANGE;

    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK) {
        return YORUNVM_ERASE_ERROR;
    }

    return YORUNVM_OK;
}
#endif

/* =========================================================
 *  API Declarations / Implementations
 * ========================================================= */

#if YORUNVM_ENABLE

static inline YORUNVM_StatusTypeDef YORUNVM_Init(uint32_t base, uint32_t size)
{
    uint32_t media_end = 0u;
    uint32_t end_addr;

#if YORUNVM_DUAL_REGION
    uint8_t region_type = 0u;

    if (!yorunvm__detect_region_(base, &media_end, &region_type)) {
        return YORUNVM_INVALID_ADDR;
    }
#elif YORUNVM_ENABLE_FLASH
    if ((YORUNVM_FLASH_SIZE_BYTES == 0u) ||
        !yorunvm__region_contains_(YORUNVM_FLASH_BASE, YORUNVM_FLASH_BASE + YORUNVM_FLASH_SIZE_BYTES, base)) {
        return YORUNVM_INVALID_ADDR;
    }
    media_end = YORUNVM_FLASH_BASE + YORUNVM_FLASH_SIZE_BYTES;
#elif YORUNVM_ENABLE_EEPROM
    if ((YORUNVM_EEPROM_SIZE_BYTES == 0u) ||
        !yorunvm__region_contains_(YORUNVM_EEPROM_BASE, YORUNVM_EEPROM_BASE + YORUNVM_EEPROM_SIZE_BYTES, base)) {
        return YORUNVM_INVALID_ADDR;
    }
    media_end = YORUNVM_EEPROM_BASE + YORUNVM_EEPROM_SIZE_BYTES;
#endif

    if (size == 0u) {
        end_addr = media_end;
    } else {
        end_addr = base + size;
        if ((end_addr < base) || (end_addr > media_end)) {
            return YORUNVM_RANGE_ERROR;
        }
    }

    hYorunvm.BaseAddr = base;
    hYorunvm.EndAddr = end_addr;
    hYorunvm.Initialized = 1u;
#if YORUNVM_DUAL_REGION
    hYorunvm.RegionType = region_type;
#endif

    return YORUNVM_OK;
}

#if YORUNVM_ENABLE_FLASH
static inline YORUNVM_StatusTypeDef YORUNVM_FlashRead(uint32_t addr, void *buf, uint32_t len)
{
    YORUNVM_StatusTypeDef st;

    if ((buf == (void *)0) && (len != 0u)) {
        return YORUNVM_INVALID_PARAM;
    }

    st = yorunvm__require_init_(1u, addr, len);
    if (st != YORUNVM_OK) {
        return st;
    }

    yorunvm__copy_from_addr_(buf, addr, len);
    return YORUNVM_OK;
}

static inline YORUNVM_StatusTypeDef YORUNVM_FlashWrite(uint32_t addr, const void *buf, uint32_t len)
{
    YORUNVM_StatusTypeDef st;
    const uint8_t *src = (const uint8_t *)buf;
    uint32_t cur;

    if ((buf == (const void *)0) && (len != 0u)) {
        return YORUNVM_INVALID_PARAM;
    }

    if ((YORUNVM_FLASH_WRITE_UNIT == 0u) ||
        ((addr % YORUNVM_FLASH_WRITE_UNIT) != 0u) ||
        ((len % YORUNVM_FLASH_WRITE_UNIT) != 0u)) {
        return YORUNVM_ALIGNMENT_ERROR;
    }

    st = yorunvm__require_init_(1u, addr, len);
    if (st != YORUNVM_OK) {
        return st;
    }

    YORUNVM_LOCK();
    if (HAL_FLASH_Unlock() != HAL_OK) {
        YORUNVM_UNLOCK();
        return YORUNVM_ERROR;
    }

    for (cur = 0u; cur < len; cur += YORUNVM_FLASH_WRITE_UNIT) {
        if (YORUNVM_FLASH_PROGRAM_CALL(addr + cur, src + cur) != HAL_OK) {
            (void)HAL_FLASH_Lock();
            YORUNVM_UNLOCK();
            return YORUNVM_WRITE_ERROR;
        }
    }

    (void)HAL_FLASH_Lock();
    YORUNVM_UNLOCK();
    return YORUNVM_OK;
}

static inline YORUNVM_StatusTypeDef YORUNVM_FlashErase(uint32_t addr, uint32_t len)
{
    YORUNVM_StatusTypeDef st;
    uint32_t start_sector_abs;
    uint32_t end_sector_abs;
    uint32_t bank1_sector_count;
    uint32_t first_count;
    uint32_t second_count;

    if ((YORUNVM_FLASH_SECTOR_SIZE == 0u) || (YORUNVM_FLASH_SECTORS_PER_BANK == 0u) || (len == 0u)) {
        return (len == 0u) ? YORUNVM_OK : YORUNVM_UNSUPPORTED;
    }

    st = yorunvm__require_init_(1u, addr, len);
    if (st != YORUNVM_OK) {
        return st;
    }

    start_sector_abs = (addr - YORUNVM_FLASH_BASE) / YORUNVM_FLASH_SECTOR_SIZE;
    end_sector_abs = (addr + len - 1u - YORUNVM_FLASH_BASE) / YORUNVM_FLASH_SECTOR_SIZE;
    bank1_sector_count = YORUNVM_FLASH_BANK_SIZE / YORUNVM_FLASH_SECTOR_SIZE;

    YORUNVM_LOCK();
    if (HAL_FLASH_Unlock() != HAL_OK) {
        YORUNVM_UNLOCK();
        return YORUNVM_ERROR;
    }

    if (end_sector_abs < bank1_sector_count) {
        st = yorunvm__flash_erase_one_bank_(FLASH_BANK_1, start_sector_abs, end_sector_abs - start_sector_abs + 1u);
    } else if (start_sector_abs >= bank1_sector_count) {
        st = yorunvm__flash_erase_one_bank_(FLASH_BANK_2,
                                            start_sector_abs - bank1_sector_count,
                                            end_sector_abs - start_sector_abs + 1u);
    } else {
        first_count = bank1_sector_count - start_sector_abs;
        second_count = end_sector_abs - bank1_sector_count + 1u;

        st = yorunvm__flash_erase_one_bank_(FLASH_BANK_1, start_sector_abs, first_count);
        if (st == YORUNVM_OK) {
            st = yorunvm__flash_erase_one_bank_(FLASH_BANK_2, 0u, second_count);
        }
    }

    (void)HAL_FLASH_Lock();
    YORUNVM_UNLOCK();
    return st;
}
#endif

#if YORUNVM_ENABLE_EEPROM
static inline YORUNVM_StatusTypeDef YORUNVM_EepromRead(uint32_t addr, void *buf, uint32_t len)
{
    YORUNVM_StatusTypeDef st;

    if ((buf == (void *)0) && (len != 0u)) {
        return YORUNVM_INVALID_PARAM;
    }

    st = yorunvm__require_init_(2u, addr, len);
    if (st != YORUNVM_OK) {
        return st;
    }

    yorunvm__copy_from_addr_(buf, addr, len);
    return YORUNVM_OK;
}

static inline YORUNVM_StatusTypeDef YORUNVM_EepromWrite(uint32_t addr, const void *buf, uint32_t len)
{
    (void)addr;
    (void)buf;
    (void)len;
    return YORUNVM_UNSUPPORTED;
}
#endif

#endif /* YORUNVM_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* YORUNVM_H */
