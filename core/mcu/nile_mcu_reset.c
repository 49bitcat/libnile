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
#define MCU_RESET_WAIT_TIME (48 * 4)

bool nile_mcu_reset(bool to_bootloader) {
    if (!nile_spi_wait_ready())
        return false;

    // Hack! SPI output is connected to BOOT0.
    outportw(IO_NILE_SPI_CNT, NILE_SPI_BUFFER_IDX);
    memset(MEM_NILE_SPI_BUFFER, to_bootloader ? 0xFF : 0x00, MCU_RESET_WAIT_TIME);
    outportw(IO_NILE_SPI_CNT, NILE_SPI_START | NILE_SPI_MODE_WRITE | NILE_SPI_CLOCK_CART | NILE_SPI_DEV_NONE | (MCU_RESET_WAIT_TIME - 1));

    // Wait a few cycles to trigger RESET.
    uint8_t irq = inportb(IO_NILE_IRQ) & ~NILE_IRQ_MCU_RESET;
    ws_busywait(1000);
    outportb(IO_NILE_IRQ, irq);
    ws_busywait(1000);
    outportb(IO_NILE_IRQ, irq | NILE_IRQ_MCU_RESET);

    if (!nile_spi_wait_ready())
        return false;

    if (to_bootloader) {
        ws_busywait(5000);
        outportw(IO_NILE_SPI_CNT, NILE_SPI_DEV_MCU | NILE_SPI_CLOCK_CART);
        nile_spi_xch(NILE_MCU_BOOT_START);
        return nile_mcu_boot_wait_ack();
    }

    return true;
}
