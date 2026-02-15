/*
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

#ifndef NILE_FLASH_LAYOUT_H_
#define NILE_FLASH_LAYOUT_H_

#include <wonderful.h>

#define NILE_FLASH_LAYOUT_FPGA_FACTORY_ADDR     0x000000
#define NILE_FLASH_LAYOUT_IPL1_FACTORY_ADDR     0x008000
#define NILE_FLASH_LAYOUT_IPL1_RECOVERY_ADDR    0x00C000
#define NILE_FLASH_LAYOUT_MANIFEST_FACTORY_ADDR 0x00F000
#define NILE_FLASH_LAYOUT_RECOVERY_FACTORY_ADDR 0x010000
#define NILE_FLASH_LAYOUT_FACTORY_SIZE          0x040000

#define NILE_FLASH_LAYOUT_FPGA_SIZE             0x008000
#define NILE_FLASH_LAYOUT_IPL1_ADDR             0x040000
#define NILE_FLASH_LAYOUT_IPL1_SIZE             0x004000
#define NILE_FLASH_LAYOUT_MANIFEST_ADDR         0x04F000
#define NILE_FLASH_LAYOUT_RECOVERY_ADDR         0x050000
#define NILE_FLASH_LAYOUT_RECOVERY_SIZE         0x030000
#define NILE_FLASH_LAYOUT_FPGA0_ADDR            0x080000
#define NILE_FLASH_LAYOUT_FPGA1_ADDR            0x088000
#define NILE_FLASH_LAYOUT_FPGA2_ADDR            0x090000
#define NILE_FLASH_LAYOUT_FPGA3_ADDR            0x098000

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>

#define NILE_FLASH_MANIFEST_ID 0x5746

/**
 * @brief Firmware flash version data structure.
 */
typedef struct __attribute__((packed)) {
	uint16_t id; ///< Magic value, should be equal to NILE_FLASH_MANIFEST_ID
	uint16_t major; ///< Major version number.
	uint16_t minor; ///< Minor version number.
	uint16_t patch; ///< Patch version number.
	uint8_t reserved[3];
	uint8_t partial_install; ///< 0x00 if install successful, non-0x00 if partial
} nile_flash_version_t;

/**
 * @brief Firmware flash version manifest data structure.
 */
typedef struct __attribute__((packed)) {
	nile_flash_version_t version;
	uint8_t commit_id[20];
	uint8_t digest[32];
} nile_flash_manifest_t;

static inline bool nile_flash_layout_read_version(void __far* buffer, size_t size) {
	return nile_flash_read(buffer, NILE_FLASH_LAYOUT_MANIFEST_ADDR, size);
}

static inline bool nile_flash_layout_read_version_factory(void __far* buffer, size_t size) {
	return nile_flash_read(buffer, NILE_FLASH_LAYOUT_MANIFEST_FACTORY_ADDR, size);
}

bool nile_flash_layout_version_at_least(uint16_t major, uint16_t minor, uint16_t patch);
bool nile_flash_layout_version_compatible(uint16_t major, uint16_t minor, uint16_t patch, uint16_t first_incompatible_major);

#endif

#endif /* NILE_FLASH_LAYOUT_H_ */
