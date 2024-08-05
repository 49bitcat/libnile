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

	.arch	i186
	.code16
	.intel_syntax noprefix

    .section .fartext.s.nile_tf_wait_ready, "ax"
    .align 2
    .global nile_tf_wait_ready
nile_tf_wait_ready:
    mov dx, ax
    push ds
    push si

    // volatile uint16_t prev_bank = inportw(IO_BANK_2003_ROM1);
    in ax, IO_BANK_2003_ROM1
    push ax
    mov ax, NILE_SEG_ROM_SPI_RX
    out IO_BANK_2003_ROM1, ax

    push 0x3000
    pop ds

    mov bx, 23
    xor cx, cx
    xor si, si

nile_tf_wait_ready_is_spi_ready:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js nile_tf_wait_ready_is_spi_ready
    and ax, 0x7800
    or ah, (NILE_SPI_MODE_READ >> 8)
    out IO_NILE_SPI_CNT, ax

nile_tf_wait_ready_loop:
    in ax, IO_NILE_SPI_CNT
    or ah, (NILE_SPI_START >> 8)
    out IO_NILE_SPI_CNT, ax

nile_tf_wait_ready_is_spi_ready2:
    in ax, IO_NILE_SPI_CNT
    test ah, (NILE_SPI_BUSY >> 8)
    jnz nile_tf_wait_ready_is_spi_ready2

    xor ah, (NILE_SPI_BUFFER_IDX >> 8)
    out IO_NILE_SPI_CNT, ax

    cmp byte ptr [si], 0xFF     // is SPI ready?
    je nile_tf_wait_ready_done

    dec cx
    jnz nile_tf_wait_ready_loop
    dec bx
    jnz nile_tf_wait_ready_loop

    // timeout
    mov dx, 0x00FF

nile_tf_wait_ready_done:
    // outportw(IO_BANK_2003_ROM1, prev_bank);
    pop ax
    out IO_BANK_2003_ROM1, ax

    mov ax, dx

    pop si
    pop ds
    ASM_PLATFORM_RET


    .section .fartext.s.nile_tf_cs_high, "ax"
    .align 2
    .global nile_tf_cs_high
nile_tf_cs_high:
    ASM_PLATFORM_CALL nile_spi_wait_ready
    test al, al
    jz nile_tf_cs_high_ret

    in ax, IO_NILE_SPI_CNT
    and ax, NILE_SPI_CFG_MASK
    or ah, ((NILE_SPI_DEV_NONE | NILE_SPI_START | NILE_SPI_MODE_READ) >> 8) // pull CS high
    out IO_NILE_SPI_CNT, ax

nile_tf_cs_high2:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js nile_tf_cs_high2

    mov al, 1
nile_tf_cs_high_ret:
    ASM_PLATFORM_RET


    .section .fartext.s.nile_tf_cs_low, "ax"
    .align 2
    .global nile_tf_cs_low
nile_tf_cs_low:
    ASM_PLATFORM_CALL nile_spi_wait_ready
    test al, al
    jz nile_tf_cs_low_ret

    in ax, IO_NILE_SPI_CNT
    and ax, NILE_SPI_CFG_MASK
    or ah, ((NILE_SPI_DEV_TF | NILE_SPI_START | NILE_SPI_MODE_READ) >> 8) // pull CS low
    out IO_NILE_SPI_CNT, ax

nile_tf_cs_low2:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js nile_tf_cs_low2

    xor ax, ax
    ASM_PLATFORM_CALL nile_tf_wait_ready
    test al, al
    mov al, 0
    jnz nile_tf_cs_low_ret
    mov al, 1

nile_tf_cs_low_ret:
    ASM_PLATFORM_RET
    
    .section .fartext.s.nile_disk_read_inner, "ax"
    .align 2
__read256:
#if 0
    push cx
    mov cx, 0x80
    rep movsw
    pop cx
#else
.rept 128
    movsw
.endr
#endif
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
    mov di, ax  // write to ES:DI
    mov es, dx

    // volatile uint16_t prev_bank = inportw(IO_BANK_2003_ROM1);
    in ax, IO_BANK_2003_ROM1
    push ax
    mov ax, NILE_SEG_ROM_SPI_RX
    out IO_BANK_2003_ROM1, ax

    __waitread1

    push 0x3000 // read from 0x3000:0x0000 
    pop ds
nile_disk_read_inner_loop:
    xor si, si

    // wait for read(1) to end
    ASM_PLATFORM_CALL nile_spi_wait_ready
    test al, al
    jz nile_disk_read_inner_done

    in ax, IO_NILE_SPI_CNT
    xor ax, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    // resp[0] == 0xFE?
    cmp byte ptr [si], 0xFE
    mov al, 0
    jne nile_disk_read_inner_done

    // queue read 256 bytes
    and ah, (NILE_SPI_CFG_MASK >> 8)
    or ax, (0xFF | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

nile_disk_read_inner_loop_s1:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js nile_disk_read_inner_loop_s1

    // queue read 258 bytes
    and ax, NILE_SPI_CFG_MASK
    xor ax, (0x101 | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ)
    out IO_NILE_SPI_CNT, ax

    // read 256 bytes
    call __read256
    xor si, si

nile_disk_read_inner_loop_s2:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js nile_disk_read_inner_loop_s2

    xor ah, (NILE_SPI_BUFFER_IDX >> 8)
    out IO_NILE_SPI_CNT, ax

    // read 256 bytes
    dec bx
    jz nile_disk_read_inner_success
    __waitread1
    call __read256
    jmp nile_disk_read_inner_loop

nile_disk_read_inner_success:
    call __read256
    mov al, 1
nile_disk_read_inner_done:
    mov si, ax

    // outportw(IO_BANK_2003_ROM1, prev_bank);
    pop ax
    out IO_BANK_2003_ROM1, ax

    mov ax, si
    pop si
    pop ds
    pop di
    pop es
    ASM_PLATFORM_RET
