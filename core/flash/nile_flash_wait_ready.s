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
    .global nile_flash_wait_ready
nile_flash_wait_ready:
    push ds

    // Set DS to 0x1000
    push 0x1000
    pop ds

    m_push_sram_bank_state
    m_open_sram_ram_bank NILE_SEG_RAM_SPI_TX

    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

    // Write one byte, store NILE_SPI_CNT value in BX
    mov byte ptr [0x0000], NILE_FLASH_CMD_RDSR1

    in ax, IO_NILE_SPI_CNT
    and ax, (NILE_SPI_CLOCK_MASK | NILE_SPI_BUFFER_IDX)
    xor ax, (NILE_SPI_DEV_FLASH | NILE_SPI_MODE_EXCH | NILE_SPI_START | NILE_SPI_BUFFER_IDX)
    mov bx, ax
    out IO_NILE_SPI_CNT, ax
    
    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

1:
    // Keep reading bytes until BUSY is clear
    mov ax, bx
    xor bx, NILE_SPI_BUFFER_IDX
    out IO_NILE_SPI_CNT, ax

    call __nile_spi_wait_ready_near
    test al, al
    jz 9f

    mov ax, bx
    and ah, ~(NILE_SPI_START >> 8)
    out IO_NILE_SPI_CNT, ax

    test byte ptr [0x0000], NILE_FLASH_SR1_BUSY
    jnz 1b

    mov ax, 1
9:
    m_pop_sram_bank_state

    // Set CS to high
    mov ax, bx
    and ax, ~NILE_SPI_DEV_MASK
    out IO_NILE_SPI_CNT, ax

    pop ds
    WF_PLATFORM_RET
