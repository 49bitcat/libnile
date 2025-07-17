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

#include <wonderful.h>
#include <ws.h>
#include "nile.h"

bool nile_spi_set_speed(nile_spi_speed_t speed) {
    if (!nile_spi_wait_ready())
        return false;

    if (ws_system_is_color_active()) {
        uint8_t color_cfg = inportb(WS_SYSTEM_CTRL_COLOR_PORT);
        outportb(WS_SYSTEM_CTRL_COLOR_PORT,
            speed == NILE_SPI_SPEED_6MHZ ? (color_cfg | WS_SYSTEM_CTRL_COLOR_CART_FAST_CLOCK) : (color_cfg & ~WS_SYSTEM_CTRL_COLOR_CART_FAST_CLOCK));
    }

    uint16_t cfg = inportw(IO_NILE_SPI_CNT);
    outportw(IO_NILE_SPI_CNT, speed == NILE_SPI_SPEED_24MHZ ? (cfg & ~NILE_SPI_CLOCK_CART) : (cfg | NILE_SPI_CLOCK_CART));

    return true;
}
