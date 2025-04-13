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
#include "../core/macros.inc"

	.arch	i186
	.code16
	.intel_syntax noprefix

    .section .fartext.s.libnile, "ax"
    .align 2
    .global nile_tf_wait_ready
nile_tf_wait_ready:
    mov dx, ax
    push ds
    push si

#ifndef LIBNILE_CLOBBER_ROM1
    in ax, IO_BANK_2003_ROM1
    push ax
#endif
    mov ax, NILE_SEG_ROM_SPI_RX
    out IO_BANK_2003_ROM1, ax

    push 0x3000
    pop ds

    mov bx, 23
    xor cx, cx
    xor si, si

    m_nile_spi_wait_ready_ax_no_timeout
    and ax, NILE_SPI_CFG_MASK
2:
    or ax, (NILE_SPI_MODE_READ | NILE_SPI_START)
    out IO_NILE_SPI_CNT, ax

    m_nile_spi_wait_ready_ax_no_timeout

    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    cmp byte ptr [si], 0xFF // is SPI device ready?
    je 9f

    dec cx
    jnz 2b
    dec bx
    jnz 2b

    // timeout
    mov dx, 0x00FF

9:
#ifndef LIBNILE_CLOBBER_ROM1
    pop ax
    out IO_BANK_2003_ROM1, ax
#endif

    mov ax, dx

    pop si
    pop ds
    WF_PLATFORM_RET
