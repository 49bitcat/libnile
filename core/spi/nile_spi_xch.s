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
    .global nile_spi_xch
    // Input:
    // - AL = byte
nile_spi_xch:
    push ds
    push 0x1000
    pop ds

    mov bx, ax

    call __nile_spi_wait_ready_near
    test al, al
    mov ah, 0xFF
    jz 9f

    m_push_sram_bank_state
    m_open_sram_ram_bank NILE_SEG_RAM_SPI_TX

    mov [0x0000], bl

    // uint16_t cnt = inportw(IO_NILE_SPI_CNT);
    in ax, IO_NILE_SPI_CNT
    // uint16_t new_cnt = NILE_SPI_MODE_EXCH | (cnt & 0x7800);
    // outportw(IO_NILE_SPI_CNT, (new_cnt ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START)));
    and ax, NILE_SPI_CFG_MASK
    xor ah, ((NILE_SPI_MODE_EXCH | NILE_SPI_BUFFER_IDX | NILE_SPI_START) >> 8)
    out IO_NILE_SPI_CNT, ax

    m_pop_sram_bank_state

    push 0x3000
    pop ds

#ifndef LIBNILE_CLOBBER_ROM1
    // volatile uint16_t prev_bank = inportw(IO_BANK_2003_ROM1);
    in ax, IO_BANK_2003_ROM1
    mov bx, ax
#endif

    call __nile_spi_wait_ready_near
    test al, al
    mov ah, 0xFF
    jz 9f

    // outportw(IO_BANK_2003_ROM1, NILE_SEG_ROM_SPI_RX);
    mov ax, NILE_SEG_ROM_SPI_RX
    out IO_BANK_2003_ROM1, ax

    // flip SPI buffer
    in al, (IO_NILE_SPI_CNT+1)
    xor al, (NILE_SPI_BUFFER_IDX >> 8)
    out (IO_NILE_SPI_CNT+1), al

    xor ax, ax
    mov al, [0x0000]

#ifndef LIBNILE_CLOBBER_ROM1
    // outportw(IO_BANK_2003_ROM1, prev_bank);
    xchg ax, bx
    out IO_BANK_2003_ROM1, ax
    xchg ax, bx
#endif

9:
    pop ds
    WF_PLATFORM_RET
