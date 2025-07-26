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

#ifndef NILE_FLASH_H_
#define NILE_FLASH_H_

#include <wonderful.h>
#include "hardware.h"

#define NILE_FLASH_ID_W25Q16JV_IQ 0xEF4015
#define NILE_FLASH_ID_W25Q16JV_IM 0xEF7015

#define NILE_FLASH_SR1_BUSY      (1 << 0) ///< Erase/Write in Progress
#define NILE_FLASH_SR1_WEL       (1 << 1) ///< Write Enable Latch
#define NILE_FLASH_SR1_BP0       (1 << 2) ///< Block Protect 0
#define NILE_FLASH_SR1_BP1       (1 << 3) ///< Block Protect 1
#define NILE_FLASH_SR1_BP2       (1 << 4) ///< Block Protect 2
#define NILE_FLASH_SR1_TB        (1 << 5) ///< Top/Bottom Block Protect
#define NILE_FLASH_SR1_SEC       (1 << 6) ///< Sector/Block Protect
#define NILE_FLASH_SR1_CMP       (1 << 7) ///< Complement Protect

#define NILE_FLASH_SR2_SRL       (1 << 0) ///< Status Register Lock
#define NILE_FLASH_SR2_LB1       (1 << 3) ///< Security Register 1 Lock
#define NILE_FLASH_SR2_LB2       (1 << 4) ///< Security Register 2 Lock
#define NILE_FLASH_SR2_LB3       (1 << 5) ///< Security Register 3 Lock
#define NILE_FLASH_SR2_CMP       (1 << 6) ///< Complement Protect
#define NILE_FLASH_SR2_SUS       (1 << 7) ///< Suspend Status

#define NILE_FLASH_SR3_WPS       (1 << 2) ///< Write Protect Selection
#define NILE_FLASH_SR3_DRV_100   (0)      ///< Output Driver Strength 100%
#define NILE_FLASH_SR3_DRV_75    (1 << 5) ///< Output Driver Strength 75%
#define NILE_FLASH_SR3_DRV_50    (2 << 5) ///< Output Driver Strength 50%
#define NILE_FLASH_SR3_DRV_25    (3 << 5) ///< Output Driver Strength 25%
#define NILE_FLASH_SR3_DRV_MASK  (3 << 5) ///< Output Driver Strength Mask

#define NILE_FLASH_CMD_WRSR1     0x01 ///< Write Status Register 1
#define NILE_FLASH_CMD_WRITE     0x02 ///< Write Data
#define NILE_FLASH_CMD_READ      0x03 ///< Read Data
#define NILE_FLASH_CMD_WRDI      0x04 ///< Write Disable
#define NILE_FLASH_CMD_RDSR1     0x05 ///< Read Status Register 1
#define NILE_FLASH_CMD_WREN      0x06 ///< Write Enable
#define NILE_FLASH_CMD_WRSR3     0x11 ///< Write Status Register 3
#define NILE_FLASH_CMD_RDSR3     0x15 ///< Read Status Register 3
#define NILE_FLASH_CMD_ERASE_4K  0x20 ///< Sector Erase (4K)
#define NILE_FLASH_CMD_WRSR2     0x31 ///< Write Status Register 2
#define NILE_FLASH_CMD_RDSR2     0x35 ///< Read Status Register 2
#define NILE_FLASH_CMD_BLOCK_LOCK   0x36 ///< Individual block lock
#define NILE_FLASH_CMD_BLOCK_UNLOCK 0x39 ///< Individual block unlock
#define NILE_FLASH_CMD_BLOCK_RDLOCK 0x3D ///< Read block lock
#define NILE_FLASH_CMD_SEC_WRITE 0x42 ///< Write security registers
#define NILE_FLASH_CMD_SEC_ERASE 0x44 ///< Erase security registers
#define NILE_FLASH_CMD_SEC_READ  0x48 ///< Read security registers
#define NILE_FLASH_CMD_RDUUID    0x4B ///< Read 64-bit Unique ID
#define NILE_FLASH_CMD_ERASE_32K 0x52 ///< Block Erase (32K)
#define NILE_FLASH_CMD_RDSFPD    0x5A ///< Read SFDP
#define NILE_FLASH_CMD_RESET_EN  0x66 ///< Enable reset
#define NILE_FLASH_CMD_SUSPEND   0x75 ///< Erase/Program Suspend
#define NILE_FLASH_CMD_RESUME    0x7A ///< Erase/Program Resume
#define NILE_FLASH_CMD_LOCK      0x7E ///< Global lock
#define NILE_FLASH_CMD_MFR_ID    0x90 ///< Read Manufacturer / Device ID
#define NILE_FLASH_CMD_UNLOCK    0x98 ///< Global unlock
#define NILE_FLASH_CMD_RESET     0x99 ///< Reset device
#define NILE_FLASH_CMD_RDID      0x9F ///< Read JEDEC ID
#define NILE_FLASH_CMD_WAKE_ID   0xAB ///< Release Power-down / Device ID
#define NILE_FLASH_CMD_SLEEP     0xB9 ///< Power-down
#define NILE_FLASH_CMD_ERASE_ALL 0xC7 ///< Chip Erase. Not advisable
#define NILE_FLASH_CMD_ERASE_64K 0xD8 ///< Block Erase (64K)

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Unlock global SPI flash writing.
 */
static inline bool nile_flash_write_unlock_global(void) {
    bool __nile_flash_cmd(uint8_t cmd);
    return __nile_flash_cmd(NILE_FLASH_CMD_WREN) && __nile_flash_cmd(NILE_FLASH_CMD_UNLOCK);
}

/**
 * @brief Enable SPI flash writing.
 */
static inline bool nile_flash_write_enable(void) {
    bool __nile_flash_cmd(uint8_t cmd);
    return __nile_flash_cmd(NILE_FLASH_CMD_WREN);
}

/**
 * @brief Disable SPI flash writing.
 */
static inline bool nile_flash_write_disable(void) {
    bool __nile_flash_cmd(uint8_t cmd);
    return __nile_flash_cmd(NILE_FLASH_CMD_WRDI);
}

/**
 * @brief Wake up SPI flash.
 */
static inline bool nile_flash_wake(void) {
    bool __nile_flash_cmd(uint8_t cmd);
    return __nile_flash_cmd(NILE_FLASH_CMD_WAKE_ID);
}

/**
 * @brief Put SPI flash to sleep.
 */
static inline bool nile_flash_sleep(void) {
    bool __nile_flash_cmd(uint8_t cmd);
    return __nile_flash_cmd(NILE_FLASH_CMD_SLEEP);
}

/**
 * @brief Read device UUID (8 bytes) from SPI flash.
 */
bool nile_flash_read_uuid(uint8_t *buffer);

/**
 * @brief Read JEDEC ID from SPI flash.
 */
uint32_t nile_flash_read_id(void);

/**
 * @brief Read data from SPI flash.
 * 
 * @param buffer Buffer to read to
 * @param address Address to read from
 * @param size Length of data to be read
 */
bool nile_flash_read(void __far* buffer, uint32_t address, uint16_t size);

/**
 * @brief Write page to SPI flash.
 * 
 * @param buffer Buffer to write from
 * @param address Address to write to
 * @param size Length of data to be written
 */
bool nile_flash_write_page(const void __far* buffer, uint32_t address, uint16_t size);

/**
 * @brief Erase area from SPI flash.
 * 
 * @param type Type (NILE_FLASH_CMD_ERASE_4K, NILE_FLASH_CMD_ERASE_32K, NILE_FLASH_CMD_ERASE_64K)
 * @param address Starting adddress
 * @return true Write succeeded.
 * @return false Write timed out.
 */
static inline bool nile_flash_erase_part(uint8_t type, uint32_t address) {
    bool __nile_flash_erase_address(uint32_t address);
    return __nile_flash_erase_address((((uint32_t) type) << 24) | address);
}

static inline uint8_t nile_flash_read_sr1(void) {
    uint8_t __nile_flash_read_sr(uint8_t value);
    return __nile_flash_read_sr(NILE_FLASH_CMD_RDSR1);
}

static inline uint8_t nile_flash_read_sr2(void) {
    uint8_t __nile_flash_read_sr(uint8_t value);
    return __nile_flash_read_sr(NILE_FLASH_CMD_RDSR2);
}

static inline uint8_t nile_flash_read_sr3(void) {
    uint8_t __nile_flash_read_sr(uint8_t value);
    return __nile_flash_read_sr(NILE_FLASH_CMD_RDSR3);
}

static inline bool nile_flash_write_sr1(uint8_t value) {
    bool __nile_flash_write_sr(uint16_t value);
    return __nile_flash_write_sr(NILE_FLASH_CMD_WRSR1 | (value << 8));
}

static inline bool nile_flash_write_sr2(uint8_t value) {
    bool __nile_flash_write_sr(uint16_t value);
    return __nile_flash_write_sr(NILE_FLASH_CMD_WRSR2 | (value << 8));
}

static inline bool nile_flash_write_sr3(uint8_t value) {
    bool __nile_flash_write_sr(uint16_t value);
    return __nile_flash_write_sr(NILE_FLASH_CMD_WRSR3 | (value << 8));
}

/**
 * @brief Wait until SPI flash is ready.
 */
bool nile_flash_wait_ready(void);

#endif /* __ASSEMBLER__ */

#endif /* NILE_FLASH_H_ */
