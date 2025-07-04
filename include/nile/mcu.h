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

#ifndef NILE_MCU_H_
#define NILE_MCU_H_

#include <wonderful.h>
#include "hardware.h"

#define NILE_MCU_BOOT_ACK          0x79
#define NILE_MCU_BOOT_NACK         0x1F
#define NILE_MCU_BOOT_START        0x5A
#define NILE_MCU_BOOT_GET          0x00
#define NILE_MCU_BOOT_GET_VERSION  0x01
#define NILE_MCU_BOOT_GET_ID       0x02
#define NILE_MCU_BOOT_READ_MEMORY  0x11
#define NILE_MCU_BOOT_JUMP         0x21
#define NILE_MCU_BOOT_WRITE_MEMORY 0x31
#define NILE_MCU_BOOT_ERASE_MEMORY 0x44
#define NILE_MCU_BOOT_SPECIAL      0x50
#define NILE_MCU_BOOT_EXT_SPECIAL  0x51
#define NILE_MCU_BOOT_WRITE_LOCK   0x63
#define NILE_MCU_BOOT_WRITE_UNLOCK 0x73
#define NILE_MCU_BOOT_READ_LOCK    0x82
#define NILE_MCU_BOOT_READ_UNLOCK  0x92
#define NILE_MCU_BOOT_GET_CRC      0xA1

#define NILE_MCU_BOOT_ERASE_ALL_SECTORS 0xFFFF

#define NILE_MCU_BOOT_FLAG_SIZE     0x01
#define NILE_MCU_BOOT_FLAG_CHECKSUM 0x02

#define NILE_MCU_FLASH_START 0x08000000
#define NILE_MCU_FLASH_PAGE_SIZE 2048

#define NILE_MCU_NATIVE_CMD(cmd, arg) (((cmd) & 0x7F) | ((arg) << 7))

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Reset the MCU.
 * 
 * @param to_bootloader If true, the MCU is restarted into bootloader mode, allowing use of the nile_mcu_boot functions. If false, the MCU is restarted into the existing firmware flashed on it.
 * @return true Reset successful.
 * @return false Reset failed.
 */
bool nile_mcu_reset(bool to_bootloader);

bool nile_mcu_boot_wait_ack(void);
bool nile_mcu_boot_send_cmd(uint8_t cmd);
bool nile_mcu_boot_send_data(const void __far *buffer, uint16_t len, uint8_t flags);
uint16_t nile_mcu_boot_recv_data(void __far* buffer, uint16_t buflen, uint8_t flags);

/**
 * @brief Get the version of the SPI protocol used.
 * More information is available in the Application Note AN4286.
 */
uint8_t nile_mcu_boot_get_version(void);

/**
 * @brief Get the chip ID.
 * More information is available in the Application Note AN4286.
 */
uint16_t nile_mcu_boot_get_id(void);

/**
 * @brief Request bytes from the MCU's address space.
 * 
 * @param address MCU address to read data from.
 * @param buffer Buffer to read data to.
 * @param buflen Amount of data to read, in bytes (1 - 256).
 */
bool nile_mcu_boot_read_memory(uint32_t address, void __far* buffer, uint16_t buflen);

/**
 * @brief Request that the MCU branch to a specific address in memory.
 * 
 * @param address Address to branch to.
 */
bool nile_mcu_boot_jump(uint32_t address);

/**
 * @brief Write bytes to the MCU's address space (RAM or flash memory).
 * 
 * @param address MCU address to write data to.
 * @param buffer Buffer to write data from.
 * @param buflen Amount of data to write, in bytes (1 - 256).
 */
bool nile_mcu_boot_write_memory(uint32_t address, const void __far* buffer, uint16_t buflen);

/**
 * @brief Erase pages of the MCU's flash memory.
 * 
 * @param sector_address The starting page to erase.
 * @param sector_count The number of pages to erase.
 * @see NILE_MCU_FLASH_PAGE_SIZE
 */
bool nile_mcu_boot_erase_memory(uint16_t sector_address, uint16_t sector_count);

/**
 * @brief Erase all of the MCU's flash memory.
 */
static inline bool nile_mcu_boot_erase_all_memory(void) {
    return nile_mcu_boot_erase_memory(0, NILE_MCU_BOOT_ERASE_ALL_SECTORS);
}

/**
 * @brief SPI communication error.
 */
#define NILE_MCU_NATIVE_ERROR_SPI -1

/**
 * @brief MCU communication error.
 */
#define NILE_MCU_NATIVE_ERROR_MCU -2

/**
 * @brief Send a "native protocol" MCU command.
 * 
 * @param cmd Command.
 * @param buffer Optional parameter buffer.
 * @param buflen Size of the parameter buffer (0 - 512 bytes).
 * @return int16_t 0 on success, or error code on failure.
 * @see NILE_MCU_NATIVE_CMD
 */
int16_t nile_mcu_native_send_cmd(uint16_t cmd, const void __wf_cram* buffer, int buflen);

/**
 * @brief Receive the response of a "native protocol" MCU command synchronously.
 *
 * If the response size exceeds the size of the buffer, the remaining bytes are consumed and skipped.
 * 
 * @param buffer Buffer to receive response to.
 * @param buflen The size of the buffer.
 * @return int16_t The number of bytes received.
 */
int16_t nile_mcu_native_recv_cmd(void __far* buffer, uint16_t buflen);

/**
 * @brief Start receiving the response of a "native protocol" MCU command asynchronously.
 * 
 * @param resplen The maximum size of the response.
 * @return int16_t 0 on success, or error code on failure.
 */
int16_t nile_mcu_native_recv_cmd_start(uint16_t resplen);

/**
 * @brief Finish receiving the response of a "native protocol" MCU command.
 * 
 * @param buffer Buffer to copy response to.
 * @param buflen The size of the buffer.
 * @return int16_t The number of bytes received.
 */
int16_t nile_mcu_native_recv_cmd_finish(void __far* buffer, uint16_t buflen);

typedef enum {
    NILE_MCU_NATIVE_MODE_CMD = 0x00,
    NILE_MCU_NATIVE_MODE_EEPROM = 0x01,
    NILE_MCU_NATIVE_MODE_RTC = 0x02,
    NILE_MCU_NATIVE_MODE_CDC = 0x03,
    NILE_MCU_NATIVE_MODE_STANDBY = 0xFF
} nile_mcu_native_mode_t;

static inline int16_t nile_mcu_native_mcu_switch_mode(uint8_t mode) {
    return nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x01, mode), NULL, 0);
}

static inline int16_t nile_mcu_native_mcu_get_uuid_sync(void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x03, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_cdc_read_sync(void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x40, buflen), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_cdc_write_sync(const void __wf_cram* buffer, uint16_t buflen) {
    int16_t result, bytes;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x41, buflen), buffer, buflen)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&bytes, 2)) < 0) return result;
    return bytes;
}

static inline int16_t nile_mcu_native_cdc_write_async_start(const void __wf_cram* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x41, buflen), buffer, buflen)) < 0) return result;
    return nile_mcu_native_recv_cmd_start(2);
}

static inline int16_t nile_mcu_native_cdc_write_async_finish(void) {
    int16_t result, bytes;
    if ((result = nile_mcu_native_recv_cmd_finish(&bytes, 2)) < 0) return result;
    return bytes;
}

#endif /* __ASSEMBLER__ */

#endif /* NILE_MCU_H_ */
