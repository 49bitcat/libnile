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

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>

#define NILE_IPC_TF_BLOCK 0x80
#define NILE_IPC_TF_TYPE(v) ((v) & 0x7F)
#define NILE_IPC_TF_TYPE_MMC    1
#define NILE_IPC_TF_TYPE_MMC_HC 2
#define NILE_IPC_TF_TYPE_TF     3
#define NILE_IPC_TF_TYPE_TF_HC  4

/**
 * @brief FFFF:0000 - the standard entrypoint.
 */
#define NILE_IPC_BOOT_FFFF_0000 0
/**
 * @brief 4000:0000 - the first alternate entrypoint.
 */
#define NILE_IPC_BOOT_4000_0000 1
/**
 * @brief 4000:0010 - the second alternate entrypoint.
 * Also used by the Pocket Challenge V2.
 */
#define NILE_IPC_BOOT_4000_0010 2

typedef struct __attribute__((packed)) {
	/**
	 * @brief Magic value. If set to NILE_IPC_MAGIC, the IPC area has been initialized by IPL1.
	 * @see NILE_IPC_MAGIC
	 */
	uint16_t magic;
	/**
	 * @brief TF card status.
	 * @see NILE_IPC_TF_BLOCK
	 * @see NILE_IPC_TF_TYPE
	 */
	uint8_t tf_card_status;
	/**
	 * @brief The entrypoint the cartridge was booted from.
	 * @see NILE_IPC_BOOT_FFFF_0000
	 * @see NILE_IPC_BOOT_4000_0000
	 * @see NILE_IPC_BOOT_4000_0010
	 */
	uint8_t boot_entrypoint;
	uint8_t reserved_1[4];
	/**
	 * @brief The register state at cartridge boot.
	 */
	union {
		struct {
			uint16_t ax, bx, cx, dx;
			uint16_t sp, bp, si, di;
			uint16_t ds, es, ss, flags;
		};
		uint16_t data[12];
	} boot_regs;
	/**
	 * @brief The I/O port state at cartridge boot.
	 */
	uint8_t boot_io[0xB8];
	uint8_t reserved_2[8];

	uint8_t user_area[288];
} nile_ipc_t;

#if __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(nile_ipc_t) == 512, "nile_ipc_t size not equal to 512");
#endif

#define MEM_NILE_IPC ((nile_ipc_t __far*) MK_FP(0x1000, 0x0000))

#endif /* __ASSEMBLER__ */

#endif /* __NILE_IPC_H__ */
