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

#include <wonderful.h>
#include <ws.h>
#include "nile.h"

uint16_t nile_mcu_native_recv_cmd(void __far* buffer, uint16_t buflen) {
    uint16_t len, resp_hdr;
    if (!nile_spi_rx_sync_block(&resp_hdr, 2, NILE_SPI_MODE_WAIT_READ))
        return 0;
    if (resp_hdr & 1)
        return 0;
    len = (resp_hdr >> 1) & 0x3FF;
    if (len < buflen)
        buflen = len;
    if (buflen && !nile_spi_rx_sync_block(buffer, buflen, NILE_SPI_MODE_READ))
        return 0;
    // If packet size > buffer size, skip remaining bytes
    if (len > buflen)
        nile_spi_rx_async(len - buflen, NILE_SPI_MODE_READ);
    return (buflen & 0x3FF) | (resp_hdr & ~0x3FF);
}
