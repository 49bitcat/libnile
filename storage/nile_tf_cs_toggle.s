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

	.arch	i186
	.code16
	.intel_syntax noprefix

    .section .fartext.s.libnile, "ax"
    .align 2
    .global nile_tf_cs_toggle
nile_tf_cs_toggle:
    call __nile_spi_wait_ready_near
    jz 9f

    in ax, IO_NILE_SPI_CNT
    and ax, (NILE_SPI_CFG_MASK & ~NILE_SPI_DEV_MASK)
    or ax, (NILE_SPI_DEV_NONE | NILE_SPI_START | NILE_SPI_MODE_READ) // pull CS high
    out IO_NILE_SPI_CNT, ax

    m_nile_spi_wait_ready_ax_no_timeout

    or ax, (NILE_SPI_DEV_TF | NILE_SPI_START | NILE_SPI_MODE_READ) // pull CS low
    out IO_NILE_SPI_CNT, ax

    xor ax, ax
    jmp nile_tf_wait_ready

9:
    mov ax, 1
    IA16_RET