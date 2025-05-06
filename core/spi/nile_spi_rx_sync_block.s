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
    .global nile_spi_rx_sync_block
    // Input:
    // - DX:AX = buffer
    // - CX = length
    // - [BP+2] = mode
nile_spi_rx_sync_block:
    mov bx, sp
    push ds
    push si
    push es
    push di
    push dx // used later as ES:DI
    push ax

    ss mov ax, [bx + IA16_CALL_STACK_OFFSET(0)]

    mov bx, cx
    dec cx
    or ax, cx
    IA16_CALL __nile_spi_rx_async_ax
    test al, al
    jz 9f

    // if (!nile_spi_wait_ready()) return false;
    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

    mov cx, bx
#ifndef LIBNILE_CLOBBER_ROM1
    // volatile uint16_t prev_bank = inportw(WS_CART_EXTBANK_ROM1_PORT);
    in ax, WS_CART_EXTBANK_ROM1_PORT
    xchg ax, bx
#endif
    // outportw(IO_NILE_SPI_CNT, new_cnt ^ NILE_SPI_BUFFER_IDX);
    in al, (IO_NILE_SPI_CNT+1)
    xor al, (NILE_SPI_BUFFER_IDX >> 8)
    out (IO_NILE_SPI_CNT+1), al
    // outportw(WS_CART_EXTBANK_ROM1_PORT, NILE_SEG_ROM_SPI_RX);
    mov ax, NILE_SEG_ROM_SPI_RX
    out WS_CART_EXTBANK_ROM1_PORT, ax

    // memcpy(buf, MK_FP(0x3000, 0x0000), size);
    pop di
    pop es
    push 0x3000
    pop ds
    xor si, si
    cld
    shr cx, 1
    rep movsw
    jnc 1f
    movsb
1:

#ifndef LIBNILE_CLOBBER_ROM1
    // outportw(WS_CART_EXTBANK_ROM1_PORT, prev_bank);
    xchg ax, bx
    out WS_CART_EXTBANK_ROM1_PORT, ax
#endif
    mov ax, 1

8:
    pop di
    pop es
    pop si
    pop ds
    IA16_CALL nile_spi_abort
    IA16_RET 0x2

9:
    add sp, 4 // remove pushed DX/AX
    jmp 8b
