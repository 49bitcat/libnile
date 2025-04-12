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
__read128:
.rept 64
    movsw
.endr
    ret

.macro __waitread1
    // nile_spi_rx(1, NILE_SPI_MODE_WAIT_READ)
    in ax, IO_NILE_SPI_CNT
    and ax, NILE_SPI_CFG_MASK
    or ah, ((NILE_SPI_START | NILE_SPI_MODE_WAIT_READ) >> 8)
    out IO_NILE_SPI_CNT, ax
.endm

    .global nile_disk_read_inner
nile_disk_read_inner:
    cld

    push es
    push di
    push ds
    push si

    mov bx, cx
    mov di, ax
    mov es, dx
    // ES:DI = destination pointer
    // BX = sectors to read

#ifndef LIBNILE_CLOBBER_ROM1
    in ax, IO_BANK_2003_ROM1
    push ax
#endif
    mov ax, NILE_SEG_ROM_SPI_RX
    out IO_BANK_2003_ROM1, ax

    __waitread1

    // DS = 0x3000 (read from ROM1)
    push 0x3000
    pop ds
nile_disk_read_inner_loop:
    xor si, si

    // Wait for SPI to be ready
    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

    in ax, IO_NILE_SPI_CNT
    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    // resp[0] == 0xFE?
    cmp byte ptr [si], 0xFE
    mov al, 0
    jne 9f

    // queue read 128 bytes
    and ah, (NILE_SPI_CFG_MASK >> 8)
    or ax, ((128-1) | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    m_nile_spi_wait_ready_ax_no_timeout

    // queue read 386 bytes
    and ax, NILE_SPI_CFG_MASK
    xor ax, ((386-1) | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    // read 128 bytes
    call __read128
    xor si, si

    m_nile_spi_wait_ready_ax_no_timeout

    xor ah, (NILE_SPI_BUFFER_IDX >> 8)
    out IO_NILE_SPI_CNT, ax

    // read 384 bytes
    dec bx
    jz 8f
    __waitread1
    call __read128
    call __read128
    call __read128
    jmp nile_disk_read_inner_loop

8:
    call __read128
    call __read128
    call __read128
    mov al, 1
9:
    mov si, ax

#ifndef LIBNILE_CLOBBER_ROM1
    pop ax
    out IO_BANK_2003_ROM1, ax
#endif

    mov ax, si
    pop si
    pop ds
    pop di
    pop es
    WF_PLATFORM_RET
