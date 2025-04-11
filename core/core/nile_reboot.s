/**
 * Copyright (c) 2025 Adrian "asie" Siekierka
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
    .global nile_reboot
nile_reboot:
    // disable interrupts
    cli

    // unlock nileswan registers
    mov al, NILE_POW_UNLOCK
    out IO_NILE_POW_CNT, al

    // disable NMI
    xor ax, ax
    out 0xB7, al
    // hide display
    out 0x00, ax

    // push 0xFFFF
    dec ax
    push ax
    // push 0x0000
    inc ax
    push ax
    dec ax

    // jump to 0xFFFF:0x0000
    // this code takes advantage of V30MZ prefetch
    out 0xC0, al
    out IO_NILE_SEG_MASK, ax
    retf
