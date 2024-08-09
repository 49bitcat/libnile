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

bool nile_mcu_boot_read_memory(uint32_t address, void __far* buffer, uint16_t buflen) {
    uint8_t cbuffer[4];
    cbuffer[0] = address >> 24;
    cbuffer[1] = address >> 16;
    cbuffer[2] = address >> 8;
    cbuffer[3] = address;

    if (!nile_mcu_boot_send_cmd(NILE_MCU_BOOT_READ_MEMORY))
        return false;

    if (!nile_mcu_boot_send_data(cbuffer, 4, NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    cbuffer[0] = buflen - 1;

    if (!nile_mcu_boot_send_data(cbuffer, 1, NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    uint16_t readlen = nile_mcu_boot_recv_data(buffer, buflen, 0);
    return readlen == buflen;
}
