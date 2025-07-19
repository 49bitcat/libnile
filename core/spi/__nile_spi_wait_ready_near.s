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

    // Wait for SPI to become ready, minding the timeout.
    // Corrupts DX, CX, AX.

    .section .fartext.s.libnile, "ax"
    .align 2
    .global __nile_spi_wait_ready_near
__nile_spi_wait_ready_near:
#ifndef __IA16_CMODEL_IS_FAR_TEXT
    .global nile_spi_wait_ready
nile_spi_wait_ready:
#endif
    mov dx, [__nile_spi_timeout]

1:
    // inner loop iterations for ~1ms
    mov cx, 238

    .balign 2, 0x90
2:
    in al, (IO_NILE_SPI_CNT + 1) // 6 cycles
    and ax, (NILE_SPI_BUSY >> 8) // 1 cycle
    jz 9f
    loop 2b // 5 cycles

    dec dx
    jnz 1b

    // timeout, AX = 0x0080
    xor ax, ax
    ret

9:
    // success, AX = 0x0000
    inc ax
    ret
