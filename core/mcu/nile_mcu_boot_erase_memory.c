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
#include "nile/mcu.h"

bool nile_mcu_boot_erase_memory(uint16_t sector_address, uint16_t sector_count) {
    uint8_t buffer[2];
    uint8_t checksum = 0x00;
    uint16_t sent_sector_count = sector_count < 0xFFF0 ? (sector_count - 1) : sector_count;

    if (!nile_mcu_boot_send_cmd(NILE_MCU_BOOT_ERASE_MEMORY))
        return false;

    buffer[0] = sent_sector_count >> 8;
    buffer[1] = sent_sector_count;

    if (!nile_mcu_boot_send_data(buffer, 2, NILE_MCU_BOOT_FLAG_CHECKSUM))
        return false;

    if (sector_count < 0xFFF0) {
        while (sector_count--) {
            buffer[0] = sector_address >> 8;
            buffer[1] = sector_address;
            if (!nile_spi_tx_async_block(buffer, 2))
                return false;
            checksum ^= sector_address >> 8;
            checksum ^= sector_address;
            sector_address++;
        }
        
        buffer[0] = checksum;
        if (!nile_spi_tx_async_block(buffer, 1))
            return false;
    }

    return nile_mcu_boot_wait_ack();
}
