# Yorunvm

Yorunvm is a lightweight single-header NVM helper for STM32 on-chip non-volatile memory.

The current stage focuses on STM32 internal Flash access. The goal is not to build a large storage framework, but to provide a small, predictable, and easy-to-integrate API for on-chip NVM access with clear range protection.

It does not depend on `printf`, dynamic memory allocation, or file-system style abstractions.

Repository:

* [ExMikuPro/Yorunvm](https://github.com/ExMikuPro/Yorunvm.git)

---

> **Yoru Series**
>
> A family of lightweight utility libraries for STM32 HAL. Each library can be used independently or combined as needed.
>
> | Library | Role |
> | --- | --- |
> | [Yorulog](https://github.com/ExMikuPro/Yorulog) | Lightweight UART logger |
> | [Yorush](https://github.com/ExMikuPro/Yorush) | Lightweight UART shell / command parser |
> | [Yorunvm](https://github.com/ExMikuPro/Yorunvm) | STM32 on-chip NVM / Flash / EEPROM access helper |
> | [Yorukv](https://github.com/ExMikuPro/Yorukv) | Lightweight KV configuration library |
> | [Yorubench](https://github.com/ExMikuPro/Yorubench) | Lightweight performance measurement library |
> | [Yoruassert](https://github.com/ExMikuPro/Yoruassert.git) | Lightweight assertion helper |

---

## Current Scope

Current stage:

* STM32 on-chip Flash read
* STM32 on-chip Flash write
* STM32 on-chip Flash erase
* user-defined valid NVM region through `YORUNVM_Init(base, size)`
* compile-time feature selection for:
  * Flash only
  * EEPROM only
  * Flash + EEPROM

Not included in the current stage:

* external SPI Flash
* QSPI / OSPI Flash
* SD card
* file system
* KV layer
* wear leveling
* redundancy / journaling
* checksum / CRC policy
* EEPROM emulation in Flash

---

## Design Goals

* single header
* small
* predictable
* no unnecessary stdlib runtime dependency
* feature control decided by user macros

Yorunvm is intended to be a low-level STM32 on-chip NVM helper, not a universal storage stack.

---

## Quick Start

Place `yorunvm.h` into your project, for example:

```text
/Core/Yorunvm/yorunvm.h
```

In exactly one `.c` file:

```c
#define YORUNVM_DEFINE_GLOBALS
#include "yorunvm.h"
```

In all other `.c` files:

```c
#include "yorunvm.h"
```

---

## Feature Modes

Yorunvm supports three compile-time modes.

### 1. Flash only

```c
#define YORUNVM_ENABLE_FLASH 1
#define YORUNVM_ENABLE_EEPROM 0
```

Only Flash APIs are exposed.

### 2. EEPROM only

```c
#define YORUNVM_ENABLE_FLASH 0
#define YORUNVM_ENABLE_EEPROM 1
```

Only EEPROM APIs are exposed.

### 3. Flash + EEPROM

```c
#define YORUNVM_ENABLE_FLASH 1
#define YORUNVM_ENABLE_EEPROM 1
```

Both API groups are exposed.

At least one of the two feature macros must be enabled.

---

## Initialization Model

```c
YORUNVM_StatusTypeDef YORUNVM_Init(uint32_t base, uint32_t size);
```

Meaning:

* `base`: first allowed address managed by Yorunvm
* `size`: allowed region length
* `size == 0`: the region extends from `base` to the end of the current NVM medium

After initialization, all accesses are limited to that region.

If an address or access length goes outside the configured region, Yorunvm returns a range error instead of touching memory outside the allowed window.

---

## Flash API

```c
YORUNVM_StatusTypeDef YORUNVM_FlashRead(uint32_t addr, void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_FlashWrite(uint32_t addr, const void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_FlashErase(uint32_t addr, uint32_t len);
```

Current behavior:

* `FlashRead` reads bytes from the configured Flash region
* `FlashWrite` does not auto-erase
* `FlashWrite` requires proper write alignment for the target platform
* `FlashErase` erases the sectors covered by the requested range

If the write is not aligned, Yorunvm returns `YORUNVM_ALIGNMENT_ERROR`.

If the region has not been initialized, Yorunvm returns `YORUNVM_NOT_INITIALIZED`.

---

## EEPROM API

When EEPROM support is enabled:

```c
YORUNVM_StatusTypeDef YORUNVM_EepromRead(uint32_t addr, void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_EepromWrite(uint32_t addr, const void *buf, uint32_t len);
```

In the current stage, the compile-time mode structure is already prepared, but the practical EEPROM path is still intentionally minimal. The Flash path is the primary finished path of this stage.

---

## Example

```c
#define YORUNVM_DEFINE_GLOBALS
#define YORUNVM_ENABLE_FLASH 1
#define YORUNVM_ENABLE_EEPROM 0
#include "yorunvm.h"

static const uint8_t data[32] = {
    0x59, 0x4F, 0x52, 0x55, 0x4E, 0x56, 0x4D, 0x31,
    0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE,
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0xA5, 0x5A, 0xC3, 0x3C, 0x0F, 0xF0, 0x69, 0x96
};

void app_nvm_demo(void)
{
    YORUNVM_Init(0x081E0000u, 0u);
    YORUNVM_FlashErase(0x081E0000u, sizeof(data));
    YORUNVM_FlashWrite(0x081E0000u, data, sizeof(data));
}
```

---

## Status Codes

Main status codes:

* `YORUNVM_OK`
* `YORUNVM_INVALID_PARAM`
* `YORUNVM_INVALID_ADDR`
* `YORUNVM_RANGE_ERROR`
* `YORUNVM_ALIGNMENT_ERROR`
* `YORUNVM_ERASE_ERROR`
* `YORUNVM_WRITE_ERROR`
* `YORUNVM_READ_ERROR`
* `YORUNVM_UNSUPPORTED`
* `YORUNVM_NOT_INITIALIZED`

---

## Notes

Yorunvm currently prefers STM32 HAL Flash routines instead of direct register-level programming.

That keeps the implementation safer and easier to verify for normal STM32 projects, even if the final code size is slightly larger than a hand-written register-only path.

At this stage, the tradeoff is intentional.

---

## License

This project is released under the MIT License.
