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

uint8_t nile_mcu_boot_get_version(void) {
    if (!nile_mcu_boot_send_cmd(NILE_MCU_BOOT_GET_VERSION))
        return 0x00;

    uint16_t result = nile_spi_xch(0x00);
    if (result & NILE_SPI_XCH_ERROR_MASK)
        return 0x00;

    if (!nile_mcu_boot_wait_ack())
        return 0x00;

    return result;
}
