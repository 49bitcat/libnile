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
#include "../core/macros.inc"
#include "internal.h"

	.arch	i186
	.code16
	.intel_syntax noprefix

    .section .fartext.s.libnile, "ax"
    .align 2

.macro __waitread1
    and ax, NILE_SPI_CFG_MASK
    or ax, (NILE_SPI_START | NILE_SPI_MODE_WAIT_READ)
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
    in ax, WS_CART_EXTBANK_ROM1_PORT
    push ax
#endif
    mov ax, NILE_SEG_ROM_SPI_RX
    out WS_CART_EXTBANK_ROM1_PORT, ax

    in ax, IO_NILE_SPI_CNT
    __waitread1

    // DS = 0x3000 (read from ROM1)
    push 0x3000
    pop ds
nile_disk_read_inner_loop:
    xor si, si

    // Wait for SPI to be ready
    call __nile_spi_wait_ready_near
    jz 9f

    in ax, IO_NILE_SPI_CNT
    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    and ax, NILE_SPI_CFG_MASK

    // resp[0] == 0xFE?
    cmp byte ptr [si], 0xFE
    jne 9f

    // queue read 144 bytes
    or ax, ((144-1) | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    m_nile_spi_wait_ready_ax_no_timeout

    // queue read 370 bytes
    and ax, NILE_SPI_CFG_MASK
    xor ax, ((370-1) | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    // read 144 bytes
    call __nile_movsw144
    xor si, si

    m_nile_spi_wait_ready_ax_no_timeout

    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    // read 370 bytes
    dec bx
    jz 8f
    __waitread1
    call __nile_movsw80
    call __nile_movsw144
    push offset nile_disk_read_inner_loop
__nile_movsw144:
.rept 32
    movsw
.endr
__nile_movsw80:
.rept 40
    movsw
.endr
    ret

8:
    call __nile_movsw80
    call __nile_movsw144
    call __nile_movsw144
    mov al, 1
9:

#ifndef LIBNILE_CLOBBER_ROM1
    mov si, ax
    pop ax
    out WS_CART_EXTBANK_ROM1_PORT, ax
    mov ax, si
#endif

    pop si
    pop ds
    pop di
    pop es
    IA16_RET

#ifdef LIBNILEFS_ENABLE_LODSW_READ
    .global __nile_lodsw512
__nile_lodsw512:
.rept 256
    lodsw
.endr
    ret

    .global nile_disk_read_inner_lodsw
nile_disk_read_inner_lodsw:
    cld

    push ds
    push si

    mov bx, dx
    mov si, ax
    // SI = destination pointer
    // BX = sectors to read

    in al, IO_NILE_EMU_CNT
    push ax

#ifndef LIBNILE_CLOBBER_ROM1
    in ax, WS_CART_EXTBANK_ROM1_PORT
    push ax
#endif
    mov ax, NILE_SEG_ROM_SPI_RX
    out WS_CART_EXTBANK_ROM1_PORT, ax

    in ax, IO_NILE_SPI_CNT
    __waitread1

    // DS = 0x3000 (read from ROM1)
    push 0x3000
    pop ds
nile_disk_read_inner_lodsw_loop:
    mov al, 0x00
    out IO_NILE_EMU_CNT, al

    // Wait for SPI to be ready
    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

    in ax, IO_NILE_SPI_CNT
    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    // resp[0] == 0xFE?
    cmp byte ptr [0x0000], 0xFE
    mov al, 0
    jne 9f

    // queue read 512 bytes
    and ax, NILE_SPI_CFG_MASK
    or ax, ((512-1) | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    mov al, NILE_EMU_LODSW_TRICK
    out IO_NILE_EMU_CNT, al

    m_nile_spi_wait_ready_ax_no_timeout

    // queue read 2 bytes
    and ax, NILE_SPI_CFG_MASK
    xor ax, ((2-1) | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    // read 512 bytes
    call __nile_lodsw512

    m_nile_spi_wait_ready_ax_no_timeout

    dec bx
    jz 8f
    __waitread1
    jmp nile_disk_read_inner_lodsw_loop

8:
    mov al, 1
9:

#ifndef LIBNILE_CLOBBER_ROM1
    mov si, ax
    pop ax
    out WS_CART_EXTBANK_ROM1_PORT, ax
    mov ax, si
#endif

    pop ax
    out IO_NILE_EMU_CNT, al

    pop si
    pop ds
    IA16_RET
#endif