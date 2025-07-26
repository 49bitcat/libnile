/**
 * Copyright (c) 2025 Adrian "asie" Siekierka
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
#include "utils.h"

uint8_t __nile_flash_read_sr(uint8_t value) {
    uint32_t result = 0;
    uint8_t cmd[2];
    cmd[0] = value;

    nile_spi_init_flash_cs_low();

    if (!nile_spi_xch_block(cmd, 2))
        goto error;

    result = cmd[1];

error:
    nile_spi_init_flash_cs_high();

    return result;
}
