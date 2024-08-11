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

#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include "nile.h"

/* 384 KHz / (48 * 8) => 48 bytes is 1ms */
#define MCU_RESET_WAIT_TIME (48 * 2)

bool nile_mcu_reset(bool to_bootloader) {
    if (!nile_spi_wait_ready())
        return false;

#ifdef LIBNILE_PCB_REV4
    // On this revision, SPI *input* is connected to BOOT0 with a pull-down resistor.
    // As a workaround, an FPGA patch ensures it's pulled high when all devices are deselected.
    outportw(IO_NILE_SPI_CNT, NILE_SPI_CLOCK_CART | (to_bootloader ? NILE_SPI_DEV_NONE : NILE_SPI_DEV_MCU));

    // Pull RESET low, then high.
    uint8_t pow = inportb(IO_NILE_POW_CNT) & ~NILE_POW_MCU_RESET;
    outportb(IO_NILE_POW_CNT, pow);
    ws_busywait(500);
    outportb(IO_NILE_POW_CNT, pow | NILE_POW_MCU_RESET);
#else
    // SPI output is connected to BOOT0. For booting from flash, pull low.
    uint8_t prev_flash = inportb(IO_CART_FLASH);
    uint16_t prev_bank = inportw(IO_BANK_2003_RAM);
    outportb(IO_CART_FLASH, 0);
    outportw(IO_BANK_2003_RAM, NILE_SEG_RAM_SPI_TX);

    outportw(IO_NILE_SPI_CNT, NILE_SPI_BUFFER_IDX | NILE_SPI_CLOCK_CART);
    memset(MEM_NILE_SPI_BUFFER, to_bootloader ? 0xFF : 0x00, MCU_RESET_WAIT_TIME);
    outportw(IO_NILE_SPI_CNT, NILE_SPI_START | NILE_SPI_MODE_WRITE | NILE_SPI_CLOCK_CART | NILE_SPI_DEV_MCU | (MCU_RESET_WAIT_TIME - 1));

    outportb(IO_CART_FLASH, prev_flash);
    outportw(IO_BANK_2003_RAM, prev_bank);

    ws_busywait(100);

    // Pull RESET low, then high.
    uint8_t pow = inportb(IO_NILE_POW_CNT) & ~NILE_POW_MCU_RESET;
    outportb(IO_NILE_POW_CNT, pow);
    ws_busywait(500);
    outportb(IO_NILE_POW_CNT, pow | NILE_POW_MCU_RESET);

    // Wait for the BOOT0 send to end.
    if (!nile_spi_wait_ready())
        return false;
#endif

    ws_busywait(10000);

    // If booting to bootloader, handle ACK.
    if (to_bootloader) {
        outportw(IO_NILE_SPI_CNT, NILE_SPI_DEV_MCU | NILE_SPI_CLOCK_CART);
        nile_spi_xch(NILE_MCU_BOOT_START);
        return nile_mcu_boot_wait_ack();
    }

    return true;
}
