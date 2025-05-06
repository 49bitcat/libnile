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
    .global nile_spi_tx_async_block
    // Input:
    // - DX:AX = buffer
    // - CX = length
nile_spi_tx_async_block:
    // Set SI and DI for later memcpy(), freeing AX
    push si
    push di
    mov si, ax
    xor di, di

    m_push_sram_bank_state
    m_open_sram_ram_bank NILE_SEG_RAM_SPI_TX

    // Decrement CX here for IO_NILE_SPI_CNT
    dec cx
    // memcpy(MK_FP(0x1000, 0x0000), buf, size);
    push cx
    push es
    push ds
    mov ds, dx
    push 0x1000
    pop es
    // CX = (CX >> 1) + 1
    // This also undoes the decrement above.
    shr cx, 1
    inc cx
    cld
    rep movsw
    pop ds
    pop es

    // if (!nile_spi_wait_ready()) return false;
    call __nile_spi_wait_ready_near

    // CX = size - 1
    pop cx
    test al, al
    jz 9f

    // uint16_t cnt = inportw(IO_NILE_SPI_CNT);
    in ax, IO_NILE_SPI_CNT
    // uint16_t new_cnt = (size - 1) | NILE_SPI_MODE_WRITE | (cnt & 0x7800);
    // outportw(IO_NILE_SPI_CNT, (new_cnt ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START)));
    and ax, NILE_SPI_CFG_MASK
    xor ax, (NILE_SPI_MODE_WRITE | NILE_SPI_BUFFER_IDX | NILE_SPI_START)
    or ax, cx
    out IO_NILE_SPI_CNT, ax

    mov ax, 1
9:
    xchg ax, dx
    m_pop_sram_bank_state
    xchg ax, dx

    pop di
    pop si
    IA16_RET
