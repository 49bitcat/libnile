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

    .section .fartext.s.libnile, "ax"
    .align 2
    .global nile_tf_cs_high
nile_tf_cs_high:
    WF_PLATFORM_CALL nile_spi_wait_ready
    test al, al
    jz 9f

    in ax, IO_NILE_SPI_CNT
    and ax, NILE_SPI_CFG_MASK
    or ah, ((NILE_SPI_DEV_NONE | NILE_SPI_START | NILE_SPI_MODE_READ) >> 8) // pull CS high
    out IO_NILE_SPI_CNT, ax

1:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js 1b

    mov al, 1
9:
    WF_PLATFORM_RET


    .section .fartext.s.libnile, "ax"
    .align 2
    .global nile_tf_cs_low
nile_tf_cs_low:
    WF_PLATFORM_CALL nile_spi_wait_ready
    test al, al
    jz 9f

    in ax, IO_NILE_SPI_CNT
    and ax, NILE_SPI_CFG_MASK
    or ah, ((NILE_SPI_DEV_TF | NILE_SPI_START | NILE_SPI_MODE_READ) >> 8) // pull CS low
    out IO_NILE_SPI_CNT, ax

1:
    in ax, IO_NILE_SPI_CNT
    test ah, ah
    js 1b

    xor ax, ax
    WF_PLATFORM_CALL nile_tf_wait_ready
    test al, al
    mov al, 0
    jnz nile_tf_cs_low_ret
    mov al, 1

9:
    WF_PLATFORM_RET
