/*
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

#ifndef NILE_FPGA_H_
#define NILE_FPGA_H_

#include <wonderful.h>
#include "hardware.h"

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Adjust banking registers and clear NILE_SEG_MASK.
 *
 * Note that if executing from ROML, this is only guaranteed safe for
 * >=1MB cartridge images, due to the linear address.
 */
void nile_bank_clear_mask(void);

/**
 * @brief Adjust banking registers and unlock NILE_SEG_MASK RAM/ROM0/ROM1.
 *
 * This variant is safe for <1MB cartridge images.
 */
void nile_bank_unlock(void);

/**
 * @brief Re-lock NILE_SEG_MASK RAM/ROM0/ROM1.
 */
void nile_bank_lock(void);

/**
 * @brief Unlock nileswan-exclusive registers.
 */
static inline void nile_io_unlock(void) {
    outportb(IO_NILE_POW_CNT, NILE_POW_UNLOCK);
}

/**
 * @brief Jump back to cartridge IPL0 (soft reset).
 *
 * Make sure to run nilefs_eject() first if you were using the TF card.
 */
void nile_soft_reset(void);

#endif /* __ASSEMBLER__ */

#endif /* NILE_FPGA_H_ */
