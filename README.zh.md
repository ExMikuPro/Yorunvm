# Yorunvm

Yorunvm 是一个面向 STM32 片上非易失性存储的轻量级单头文件辅助库。

当前阶段主要聚焦于 STM32 内部 Flash 的访问能力。它的目标不是做成一个庞大的存储框架，而是提供一套体积小、行为可预测、容易接入的片上 NVM 访问接口，并带有明确的区域保护能力。

它不依赖 `printf`、动态内存分配，也不引入文件系统风格的抽象。

仓库地址：

* [ExMikuPro/Yorunvm](https://github.com/ExMikuPro/Yorunvm.git)

---

> **Yoru 系列**
>
> 一组面向 STM32 HAL 的轻量级工具库。各库可独立使用，也可以组合使用。
>
> | 库 | 定位 |
> | --- | --- |
> | [Yorulog](https://github.com/ExMikuPro/Yorulog) | 轻量级 UART 日志库 |
> | [Yorush](https://github.com/ExMikuPro/Yorush) | 轻量级 UART Shell / 命令解析器 |
> | [Yorunvm](https://github.com/ExMikuPro/Yorunvm) | STM32 片上 NVM / Flash 访问辅助库 |
> | [Yorukv](https://github.com/ExMikuPro/Yorukv) | 轻量级 KV 配置库 |
> | [Yorubench](https://github.com/ExMikuPro/Yorubench) | 轻量级性能测量库 |
> | [Yoruassert](https://github.com/ExMikuPro/Yoruassert.git) | 轻量级断言辅助库 |

---

## 当前范围

当前阶段已完成：

* STM32 片上 Flash 读取
* STM32 片上 Flash 写入
* STM32 片上 Flash 擦除
* 通过 `YORUNVM_Init(base, size)` 定义用户允许操作的有效区域
* 编译期特性裁剪：
  * Flash only
  * EEPROM only
  * Flash + EEPROM

当前阶段不包含：

* 外部 SPI Flash
* QSPI / OSPI Flash
* SD 卡
* 文件系统
* KV 层
* 磨损均衡
* 冗余备份 / 日志式存储
* 校验和 / CRC 策略
* 基于 Flash 的 EEPROM 仿真

---

## 设计目标

* 单头文件
* 小
* 行为可预测
* 不引入不必要的标准库运行时依赖
* 功能开关由用户通过宏自己决定

Yorunvm 的定位是底层 STM32 片上 NVM 辅助库，而不是通用存储框架。

---

## 快速开始

把 `yorunvm.h` 放到你的工程中，例如：

```text
/Core/Yorunvm/yorunvm.h
```

必须且只需要在一个 `.c` 文件中：

```c
#define YORUNVM_DEFINE_GLOBALS
#include "yorunvm.h"
```

其他 `.c` 文件正常包含：

```c
#include "yorunvm.h"
```

---

## 功能模式

Yorunvm 支持三种编译期模式。

### 1. Flash only

```c
#define YORUNVM_ENABLE_FLASH 1
#define YORUNVM_ENABLE_EEPROM 0
```

只暴露 Flash API。

### 2. EEPROM only

```c
#define YORUNVM_ENABLE_FLASH 0
#define YORUNVM_ENABLE_EEPROM 1
```

只暴露 EEPROM API。

### 3. Flash + EEPROM

```c
#define YORUNVM_ENABLE_FLASH 1
#define YORUNVM_ENABLE_EEPROM 1
```

同时暴露两组 API。

这两个功能宏至少要启用一个。

---

## 初始化模型

```c
YORUNVM_StatusTypeDef YORUNVM_Init(uint32_t base, uint32_t size);
```

语义如下：

* `base`：Yorunvm 允许操作的起始地址
* `size`：允许操作的区域长度
* `size == 0`：表示从 `base` 一直到当前 NVM 介质末尾

初始化完成后，后续所有访问都会限制在这个区域内。

如果访问地址或访问长度超出配置区域，Yorunvm 会返回范围错误，而不是去触碰允许窗口之外的内存。

---

## Flash API

```c
YORUNVM_StatusTypeDef YORUNVM_FlashRead(uint32_t addr, void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_FlashWrite(uint32_t addr, const void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_FlashErase(uint32_t addr, uint32_t len);
```

当前行为：

* `FlashRead` 从配置好的 Flash 区域读取数据
* `FlashWrite` 不会自动擦除
* `FlashWrite` 需要满足目标平台的写入对齐要求
* `FlashErase` 会覆盖请求范围对应的扇区

如果写入不满足对齐要求，会返回 `YORUNVM_ALIGNMENT_ERROR`。

如果还没有初始化区域，会返回 `YORUNVM_NOT_INITIALIZED`。

---

## EEPROM API

在启用 EEPROM 支持时：

```c
YORUNVM_StatusTypeDef YORUNVM_EepromRead(uint32_t addr, void *buf, uint32_t len);
YORUNVM_StatusTypeDef YORUNVM_EepromWrite(uint32_t addr, const void *buf, uint32_t len);
```

当前阶段已经把编译期裁剪结构搭好，但实际可用路径仍然主要围绕 Flash。也就是说，第一阶段真正完成的主路径是 Flash 路径。

---

## 示例

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

## 状态码

主要状态码包括：

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

## 说明

当前版本的 Yorunvm 优先使用 STM32 HAL 的 Flash 访问路径，而不是自己直接做寄存器级写入。

这样做的好处是对一般 STM32 工程来说更安全、更容易验证，虽然最终代码体积可能会比纯手写寄存器路径稍大一些。

在当前阶段，这个取舍是有意为之的。

---

## 开源协议

本项目采用 MIT License 发布。
