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

#ifndef NILE_MCU_CDC_H_
#define NILE_MCU_CDC_H_

#include "../mcu.h"

static inline int16_t nile_mcu_native_cdc_read_sync(void __wf_cram* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x40, buflen), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(buffer, buflen);
}

static inline int16_t nile_mcu_native_cdc_write_sync(const void __far* buffer, uint16_t buflen) {
    int16_t result, bytes = 0;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x41, buflen), buffer, buflen)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&bytes, 2)) <= 0) return result;
    return bytes;
}

static inline int16_t nile_mcu_native_cdc_write_async_start(const void __far* buffer, uint16_t buflen) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x41, buflen), buffer, buflen)) < 0) return result;
    return nile_mcu_native_recv_cmd_start(2);
}
#define nile_mcu_native_cdc_write_async_finish nile_mcu_native_recv_cmd_response_int16

static inline int16_t nile_mcu_native_cdc_available_sync(void) {
    int16_t result, bytes = 0;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x43, 0), NULL, 0)) < 0) return result;
    if ((result = nile_mcu_native_recv_cmd(&bytes, 2)) <= 0) return result;
    return bytes;
}

static inline int16_t nile_mcu_native_cdc_available_async_start(void) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x43, 0), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd_start(2);
}
#define nile_mcu_native_cdc_available_async_finish nile_mcu_native_recv_cmd_response_int16

static inline int16_t nile_mcu_native_cdc_clear_sync(void) {
    int16_t result;
    if ((result = nile_mcu_native_send_cmd(NILE_MCU_NATIVE_CMD(0x44, 0xFF), NULL, 0)) < 0) return result;
    return nile_mcu_native_recv_cmd(NULL, 0);
}

#endif /* NILE_MCU_CDC_H_ */
