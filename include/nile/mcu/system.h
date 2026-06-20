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

#ifndef NILE_MCU_SYSTEM_H_
#define NILE_MCU_SYSTEM_H_

#include "../mcu.h"

/**
 * @brief Switch the mode in which the MCU is operating.
 */
static inline int16_t nile_mcu_native_mcu_switch_mode(uint8_t mode) {
    return nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_MODE, mode), NULL, 0);
}

/**
 * @brief Tell the MCU to operate at a specific SPI speed.
 *
 * This does not actually change the speed used by the cartridge by itself!
 */
static inline int16_t nile_mcu_native_mcu_spi_set_speed_sync(uint8_t speed) {
    int16_t result;
    uint8_t op_result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_FREQ, speed), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&op_result, 1)) < 1) return result;
    return op_result;
}

static inline int16_t nile_mcu_native_mcu_get_uuid_sync(void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_ID, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_mcu_get_info_sync(void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_INFO, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_mcu_reg_read_sync(uint16_t addr) {
    int16_t result;
    uint16_t value;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_REG_READ, addr), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&value, 2)) < 0) return result;
    return value;
}

static inline int16_t nile_mcu_native_mcu_reg_write_sync(uint16_t addr, uint16_t value) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_REG_WRITE, addr), &value, 2)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(NULL, 0)) < 0) return result;
    return 0;
}

static inline int16_t nile_mcu_native_mcu_get_version_sync(void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_VERSION, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_mcu_set_save_id_sync(uint16_t domain, uint32_t value) {
    int16_t result;
    bool retval = false;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_SET_SAVE_ID, domain), &value, 4)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&retval, 1)) <= 0) return result;
    return retval;
}

static inline int16_t nile_mcu_native_mcu_get_save_id_sync(uint16_t domain, uint32_t __wf_cram* value) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(NILE_MCU_NATIVE_CMD_GET_SAVE_ID, domain), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(value, 4)) < 0) return result;
    return 0;
}

#endif /* NILE_MCU_CDC_H_ */
