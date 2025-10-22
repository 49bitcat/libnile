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

bool nile_mcu_boot_write_memory(uint32_t address, const void __far* buffer, uint16_t buflen) {
    uint16_t cbuffer[2];
    cbuffer[0] = __builtin_bswap16(address >> 16);
    cbuffer[1] = __builtin_bswap16(address);

    if (!nile_mcu_boot_send_cmd(NILE_MCU_BOOT_WRITE_MEMORY))
        return false;

    if (!nile_mcu_boot_send_data(cbuffer, 4, NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    if (!nile_mcu_boot_send_data(buffer, buflen, NILE_MCU_BOOT_FLAG_SIZE | NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    return true;
}
