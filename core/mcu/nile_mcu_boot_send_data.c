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

bool nile_mcu_boot_send_data(const void __far *buffer, uint16_t len, uint8_t flags) {
    uint8_t checksum = 0x00;

    if (flags & NILE_MCU_BOOT_FLAG_SIZE) {
        checksum = len - 1;
        if (nile_spi_xch(checksum) & NILE_SPI_XCH_ERROR_MASK)
            return false;
    } else if (len == 1) {
        checksum = 0xFF;
    }

    if (!nile_spi_tx_async_block(buffer, len))
        return false;

    if (flags & NILE_MCU_BOOT_FLAG_CHECKSUM) {
        for (uint16_t i = 0; i < len; i++) {
            checksum ^= ((const uint8_t __far*) buffer)[i];
        }
        if (nile_spi_xch(checksum) & NILE_SPI_XCH_ERROR_MASK)
            return false;
    }

    return nile_mcu_boot_wait_ack();
}
