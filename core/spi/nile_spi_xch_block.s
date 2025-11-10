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
#include "macros.inc"

    .arch i186
    .code16
    .intel_syntax noprefix

    .section .fartext.s.libnile, "ax"
    .align 2
    .global nile_spi_xch_block
    // Input:
    // - DX:AX = buffer
    // - CX = length
nile_spi_xch_block:
    // Set SI and DI for later memcpy(), freeing AX
    push ax
    push cx
    push si
    push di
    mov si, ax
    xor di, di

    m_push_sram_bank_state
    m_open_sram_ram_bank NILE_SEG_RAM_SPI_TX

    // memcpy(MK_FP(0x1000, 0x0000), buf, size);
    push es
    push ds
    mov ds, dx
    push 0x1000
    pop es
    shr cx, 1
    inc cx
    cld
    rep movsw
    pop ds
    pop es

    m_pop_sram_bank_state

    pop di
    pop si
    pop cx
    pop ax

    push (NILE_SPI_MODE_EXCH | NILE_SPI_BUFFER_IDX)
    IA16_CALL_LOCAL nile_spi_rx_sync_block
    IA16_RET
