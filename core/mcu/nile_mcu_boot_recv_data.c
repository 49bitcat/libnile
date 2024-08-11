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

uint16_t nile_mcu_boot_recv_data(void __far* buffer, uint16_t buflen, uint8_t flags) {
    // Receive dummy byte
    uint16_t xch = nile_spi_xch(0x00);
    if (xch & NILE_SPI_XCH_ERROR_MASK)
        return 0;

    // Receive size
    uint16_t len = buflen;
    if (flags & NILE_MCU_BOOT_FLAG_SIZE) {
        xch = nile_spi_xch(0x00);
        if (xch & NILE_SPI_XCH_ERROR_MASK)
            return 0;
        xch++;
        len = xch > len ? len : xch;
    }

    // Receive data
    return nile_spi_rx_sync_block(buffer, len, NILE_SPI_MODE_READ);
}
