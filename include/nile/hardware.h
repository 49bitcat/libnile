/**
 * Copyright (c) 2023, 2024 Adrian "asie" Siekierka
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef __NILE_HARDWARE_H__
#define __NILE_HARDWARE_H__

#include <wonderful.h>

#define NILE_SPI_MODE_WRITE         0x0000
#define NILE_SPI_MODE_READ          0x0200
#define NILE_SPI_MODE_EXCH          0x0400
#define NILE_SPI_MODE_WAIT_READ     0x0600
#define NILE_SPI_MODE_MASK          0x0600
#define NILE_SPI_CLOCK_CART         0x0800
#define NILE_SPI_CLOCK_FAST         0x0000
#define NILE_SPI_CLOCK_MASK         0x0800
#define NILE_SPI_DEV_NONE           0x0000
#define NILE_SPI_DEV_TF             0x1000
#define NILE_SPI_DEV_FLASH          0x2000
#define NILE_SPI_DEV_MCU            0x3000
#define NILE_SPI_DEV_MASK           0x3000
#define NILE_SPI_BUFFER_IDX         0x4000
#define NILE_SPI_START              0x8000
#define NILE_SPI_BUSY               0x8000
#define NILE_SPI_CFG_MASK           (NILE_SPI_BUFFER_IDX | NILE_SPI_DEV_MASK | NILE_SPI_CLOCK_MASK)
#define IO_NILE_SPI_CNT    0xE0

#define NILE_POW_CLOCK     0x01
#define NILE_POW_TF        0x02
#define NILE_POW_IO_NILE   0x04
#define NILE_POW_IO_2001   0x08
#define NILE_POW_IO_2003   0x10
#define NILE_POW_MCU_BOOT0 0x20
#define NILE_POW_SRAM      0x40
#define NILE_POW_MCU_RESET 0x80
#define NILE_POW_UNLOCK    0xFD
#define IO_NILE_POW_CNT    0xE2

#define IO_NILE_WARMBOOT_CNT 0xE3

#define NILE_SEG_MASK_RAM_MASK  0xF
#define NILE_SEG_MASK_RAM_SHIFT 12
#define NILE_SEG_MASK_ROM_MASK  0x1FF
#define NILE_SEG_MASK_ROM_SHIFT 0
#define NILE_SEG_MASK_ROM0_LOCK (1 << 9)
#define NILE_SEG_MASK_ROM1_LOCK (1 << 10)
#define NILE_SEG_MASK_RAM_LOCK  (1 << 11)
#define NILE_SEG_MASK_LOCK      (7 << 9)
#define IO_NILE_SEG_MASK   0xE4

#define NILE_SEG_RAM_IPC    14
#define NILE_SEG_RAM_SPI_TX 15
#define NILE_SEG_ROM_SPI_RX 510
#define NILE_SEG_ROM_BOOT   511

#define NILE_EMU_EEPROM_MASK   0x3
#define NILE_EMU_EEPROM_128B   0x0
#define NILE_EMU_EEPROM_1KB    0x1
#define NILE_EMU_EEPROM_2KB    0x2
#define NILE_EMU_FLASH_FSM     0x4
#define NILE_EMU_ROM_BUS_16BIT 0x0
#define NILE_EMU_ROM_BUS_8BIT  0x8
#define IO_NILE_EMU_CNT      0xE6

#define MEM_NILE_SPI_BUFFER ((uint8_t __far*) MK_FP(0x1000, 0x0000))

#endif /* __NILE_HARDWARE_H__ */
