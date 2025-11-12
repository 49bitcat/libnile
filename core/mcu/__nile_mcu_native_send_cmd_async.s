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
    .global __nile_mcu_native_send_cmd_async
    // Input:
    // - stack = buffer
    // - DX = buflen
    // - AX = command
__nile_mcu_native_send_cmd_async:
    push bp
    mov bp, sp

    push di
    push es

    mov di, ax
    m_push_sram_bank_state
    m_open_sram_ram_bank NILE_SEG_RAM_SPI_TX
    mov ax, di

    // write command byte
    push 0x1000
    pop es
    xor di, di
    cld
    es mov [di], ax

    // wait for SPI to be ready
    push dx
    call __nile_spi_wait_ready_near
    pop dx
    jz 9f

    // keep clock; flip buffer; set device to MCU; set length to 2 - 1
    in ax, IO_NILE_SPI_CNT
    and ax, (NILE_SPI_CLOCK_MASK | NILE_SPI_BUFFER_IDX)
    xor ax, (NILE_SPI_MODE_WRITE | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_DEV_MCU | 1)
    out IO_NILE_SPI_CNT, ax

    // splitting command + argument write into two transfers slightly speeds up
    // the general case (no argument) while giving the MCU some room to handle
    // the command before doing the argument (for MHz-speed transfers)

    // is there an argument buffer?
    test dx, dx
    jz 1f

    // write buffer
    push ds
    push si
    lds si, [bp + IA16_CALL_STACK_OFFSET(2)]
    mov cx, dx
    shr cx, 1
    inc cx
    rep movsw
    pop si
    pop ds

    // wait for SPI to be ready
    push dx
    call __nile_spi_wait_ready_near
    pop dx
    jz 9f

    // keep clock; flip buffer; set device to MCU; set length to DX - 1
    in ax, IO_NILE_SPI_CNT
    and ax, (NILE_SPI_CLOCK_MASK | NILE_SPI_BUFFER_IDX)
    xor ax, (NILE_SPI_MODE_WRITE | NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_DEV_MCU)
    or ax, dx
    dec ax
    out IO_NILE_SPI_CNT, ax

1:
    mov ax, 1

9:
    mov di, ax
    m_pop_sram_bank_state
    mov ax, di

    pop es
    pop di
    pop bp

    dec ax
    IA16_RET 0x4
