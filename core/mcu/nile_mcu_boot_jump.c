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

bool nile_mcu_boot_jump(uint32_t address) {
    uint8_t buffer[4];
    buffer[0] = address >> 24;
    buffer[1] = address >> 16;
    buffer[2] = address >> 8;
    buffer[3] = address;

    if (!nile_mcu_boot_send_cmd(NILE_MCU_BOOT_JUMP))
        return false;

    if (!nile_mcu_boot_send_data(buffer, 4, NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    return true;
}
