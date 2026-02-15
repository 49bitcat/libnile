/*
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

#ifndef NILE_H_
#define NILE_H_

#include <wonderful.h>
#include "nile/hardware.h"
#include "nile/fpga.h"
#include "nile/spi.h"
#include "nile/flash.h"
#include "nile/mcu.h"
#include "nile/ipc.h"
#include "nile/flash_layout.h"

#ifndef __ASSEMBLER__
#include "nile/mcu/cdc.h"
#include "nile/mcu/eeprom.h"
#include "nile/mcu/rtc.h"
#endif /* __ASSEMBLER__ */

#endif /* NILE_H_ */
