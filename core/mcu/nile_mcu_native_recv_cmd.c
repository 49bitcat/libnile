/**
 * Copyright (c) 2023, 2024, 2025 Adrian "asie" Siekierka
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

int16_t nile_mcu_native_recv_cmd(void __far* buffer, uint16_t buflen) {
    uint16_t len, resp_hdr;

    // Read MCU response header from SPI
    if (!nile_spi_rx_sync_block(&resp_hdr, 2, NILE_SPI_MODE_WAIT_READ))
        return NILE_MCU_NATIVE_ERROR_SPI;

    if (resp_hdr & 1)
        return NILE_MCU_NATIVE_ERROR_MCU;

    // Read MCU response bytes from SPI
    len = (resp_hdr >> 1) & 0x3FF;
    if (len < buflen)
        buflen = len;
    if (buflen && !nile_spi_rx_sync_block(buffer, buflen, NILE_SPI_MODE_READ))
        return NILE_MCU_NATIVE_ERROR_SPI;

    // If packet size > buffer size, skip remaining bytes
    if (len > buflen)
        nile_spi_rx_async(len - buflen, NILE_SPI_MODE_READ);

    return (buflen & 0x3FF) | (resp_hdr & ~0x7FF);
}

int16_t nile_mcu_native_recv_cmd_start(uint16_t resplen) {
    if (!nile_spi_rx_async(2 + resplen, NILE_SPI_MODE_WAIT_READ))
        return NILE_MCU_NATIVE_ERROR_SPI;
    return 0;
}

int16_t nile_mcu_native_recv_cmd_finish(void __far* buffer, uint16_t buflen) {
    uint16_t len, resp_hdr;

    if (!nile_spi_wait_ready())
        return NILE_MCU_NATIVE_ERROR_SPI;

    int16_t result = 0;
    
    nile_spi_buffer_flip();
    ws_bank_with_rom1(NILE_SEG_ROM_SPI_RX, {
        resp_hdr = *((uint16_t __far*) MK_FP(0x3000, 0x0000));
        if (resp_hdr & 1) {
            result = NILE_MCU_NATIVE_ERROR_MCU;
        } else {
            len = (resp_hdr >> 1) & 0x3FF;
            if (len < buflen)
                buflen = len;
            if (buflen)
                memcpy(buffer, MK_FP(0x3000, 0x0002), buflen);
            result = (buflen & 0x3FF) | (resp_hdr & ~0x7FF);
        }
    });

    return result;
}