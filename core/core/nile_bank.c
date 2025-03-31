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
#include <ws/hardware.h>
#include "nile.h"

void nile_bank_clear_mask(void) {
	uint16_t mask = inportw(IO_NILE_SEG_MASK);
	uint16_t rom_mask = (mask >> NILE_SEG_MASK_ROM_SHIFT) & NILE_SEG_MASK_ROM_MASK;
	uint16_t ram_mask = (mask >> NILE_SEG_MASK_RAM_SHIFT) & NILE_SEG_MASK_RAM_MASK;

	outportw(IO_BANK_2003_RAM, inportw(IO_BANK_2003_RAM) & ram_mask);
	outportw(IO_BANK_2003_ROM0, inportw(IO_BANK_2003_ROM0) & rom_mask);
	outportw(IO_BANK_2003_ROM1, inportw(IO_BANK_2003_ROM1) & rom_mask);
	outportb(IO_BANK_ROM_LINEAR, inportb(IO_BANK_ROM_LINEAR) & (rom_mask >> 4));

	outportw(IO_NILE_SEG_MASK, 0xFFFF);
}

/**
 * @brief Adjust banking registers and unlock NILE_SEG_MASK RAM/ROM0/ROM1.
 *
 * This variant is safe for <1MB cartridge images.
 */
void nile_bank_unlock(void) {
	uint16_t mask = inportw(IO_NILE_SEG_MASK);
	uint16_t rom_mask = (mask >> NILE_SEG_MASK_ROM_SHIFT) & NILE_SEG_MASK_ROM_MASK;
	uint16_t ram_mask = (mask >> NILE_SEG_MASK_RAM_SHIFT) & NILE_SEG_MASK_RAM_MASK;

	outportw(IO_BANK_2003_RAM, inportw(IO_BANK_2003_RAM) & ram_mask);
	outportw(IO_BANK_2003_ROM0, inportw(IO_BANK_2003_ROM0) & rom_mask);
	outportw(IO_BANK_2003_ROM1, inportw(IO_BANK_2003_ROM1) & rom_mask);

	outportw(IO_NILE_SEG_MASK, inportw(IO_NILE_SEG_MASK) & ~NILE_SEG_MASK_LOCK);
}

/**
 * @brief Re-lock NILE_SEG_MASK RAM/ROM0/ROM1.
 */
void nile_bank_lock(void) {
	outportw(IO_NILE_SEG_MASK, inportw(IO_NILE_SEG_MASK) | NILE_SEG_MASK_LOCK);
}
