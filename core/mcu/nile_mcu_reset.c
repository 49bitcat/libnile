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

#define MCU_BOOT0_WAIT_TIME 50
#define MCU_RESET_WAIT_TIME 100
#define MCU_BOOT_WAIT_TIME 10000

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
    ws_busywait(MCU_RESET_WAIT_TIME);
    outportb(IO_NILE_POW_CNT, pow | NILE_POW_MCU_RESET);
#else
    // Set BOOT0 pin state. For booting from flash, pull low.
    uint8_t prev_pow_cnt = inportb(IO_NILE_POW_CNT);
    uint8_t pow_cnt = prev_pow_cnt;
    pow_cnt = to_bootloader ? (prev_pow_cnt | NILE_POW_MCU_BOOT0) : (prev_pow_cnt & ~NILE_POW_MCU_BOOT0); 
    outportb(IO_NILE_POW_CNT, pow_cnt);
    ws_busywait(MCU_BOOT0_WAIT_TIME);

    // Pull RESET low, then high.
    pow_cnt = pow_cnt & ~NILE_POW_MCU_RESET;
    outportb(IO_NILE_POW_CNT, pow_cnt);
    ws_busywait(MCU_RESET_WAIT_TIME);
    outportb(IO_NILE_POW_CNT, pow_cnt | NILE_POW_MCU_RESET);
#endif

    ws_busywait(MCU_BOOT_WAIT_TIME);

#ifndef LIBNILE_PCB_REV4
    outportb(IO_NILE_POW_CNT, prev_pow_cnt);
#endif

    // If booting to bootloader, handle ACK.
    if (to_bootloader) {
        outportw(IO_NILE_SPI_CNT, NILE_SPI_DEV_MCU | NILE_SPI_CLOCK_CART);
        nile_spi_xch(NILE_MCU_BOOT_START);
        return nile_mcu_boot_wait_ack();
    }

    return true;
}
