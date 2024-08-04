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

.macro m_push_sram_bank_state
    // uint8_t prev_flash = inportb(IO_CART_FLASH);
    in al, IO_CART_FLASH
    push ax
    // uint8_t prev_bank = inportw(IO_BANK_2003_RAM);
    in ax, IO_BANK_2003_RAM
    push ax
.endm
.macro m_pop_sram_bank_state
    // outportw(IO_BANK_2003_RAM, prev_bank);
    pop ax
    out IO_BANK_2003_RAM, ax
    // outportb(IO_CART_FLASH, prev_flash);
    pop ax
    out IO_CART_FLASH, al
.endm

.macro m_open_sram_ram_bank bank
    // outportb(IO_CART_FLASH, 0);
    xor ax, ax
    out IO_CART_FLASH, al
    // outportw(IO_BANK_2003_RAM, NILE_SEG_RAM_TX);
    mov al, \bank
    out IO_BANK_2003_RAM, ax
.endm

.macro m_open_sram_rom_bank bank
    // outportb(IO_CART_FLASH, 1);
    mov al, 1
    out IO_CART_FLASH, al
    // outportw(IO_BANK_2003_RAM, NILE_SEG_RAM_TX);
    mov ax, \bank
    out IO_BANK_2003_RAM, ax
.endm
