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

#ifndef NILE_MCU_EEPROM_H_
#define NILE_MCU_EEPROM_H_

#include "../mcu.h"

typedef enum {
    NILE_MCU_EEPROM_MODE_NONE = 0x00,
    NILE_MCU_EEPROM_MODE_M93LC06 = 0x01,
    NILE_MCU_EEPROM_MODE_M93LC46 = 0x02,
    NILE_MCU_EEPROM_MODE_M93LC56 = 0x03,
    NILE_MCU_EEPROM_MODE_M93LC66 = 0x04,
    NILE_MCU_EEPROM_MODE_M93LC76 = 0x05,
    NILE_MCU_EEPROM_MODE_M93LC86 = 0x06
} nile_mcu_eeprom_mode_t;

/**
 * @brief Set EEPROM emulation mode.
 *
 * @return int16_t 1 on success, 0 on MCU error, negative values on transfer error.
 */
static inline int16_t nile_mcu_native_eeprom_set_mode_sync(nile_mcu_eeprom_mode_t mode) {
    int16_t result;
    bool retval = false;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_MODE, mode), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&retval, 1)) <= 0) return result;
    return retval;
}

static inline int16_t nile_mcu_native_eeprom_erase_sync(void) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_ERASE, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(NULL, 0);
}

static inline int16_t nile_mcu_native_eeprom_read_sync(void __far* buffer, uint16_t offset, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_READ, buflen >> 1), &offset, 2)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_eeprom_write_sync(const void __wf_cram* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_WRITE, buflen), buffer, buflen)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(NULL, 0)) < 0) return result;
    return 0;
}

static inline int16_t nile_mcu_native_eeprom_write_async_start(const void __wf_cram* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_WRITE, buflen), buffer, buflen)) < 0) return result;
    return nile_mcu_native_recv_cmd_start(0);
}
#define nile_mcu_native_eeprom_write_async_finish nile_mcu_native_recv_cmd_response_none

/**
 * @brief Get EEPROM emulation mode.
 *
 * @return int16_t Non-negative values as in nile_mcu_eeprom_mode_t, negative values on transfer error.
 */
static inline int16_t nile_mcu_native_eeprom_get_mode_sync(void) {
    int16_t result;
    uint8_t mode;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_EEPROM_GET_MODE, 0), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&mode, 1)) <= 0) return result;
    return mode;
}

#endif /* NILE_MCU_EEPROM_H_ */
