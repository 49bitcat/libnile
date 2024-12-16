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

#ifndef __NILE_IPC_H__
#define __NILE_IPC_H__

#include <wonderful.h>

#define NILE_IPC_MAGIC 0xAA55
#define NILE_IPC_BOOT_FFFF_0000 0
#define NILE_IPC_BOOT_4000_0000 1
#define NILE_IPC_BOOT_4000_0010 2

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>

typedef struct __attribute__((packed)) {
	uint16_t magic;
	uint8_t tf_card_status;
	uint8_t boot_entrypoint;
	uint8_t reserved_1[4];
	struct {
		union {
			uint16_t ax, bx, cx, dx;
			uint16_t sp, bp, si, di;
			uint16_t ds, es, ss, flags;
		};
		uint16_t data[12];
	} boot_regs;
	uint8_t boot_io[0xB8];
	uint8_t reserved_2[8];

	uint8_t user_area[288];
} nile_ipc_t;

#define MEM_NILE_IPC ((nile_ipc_t __far*) MK_FP(0x1000, 0x0000))

#endif /* __ASSEMBLER__ */

#endif /* __NILE_IPC_H__ */
