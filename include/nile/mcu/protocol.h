/*
 * Copyright (c) 2026 Adrian "asie" Siekierka
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

#ifndef NILE_MCU_PROTOCOL_H_
#define NILE_MCU_PROTOCOL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef enum {
    NILE_MCU_NATIVE_CMD_ECHO = 0x00,
    NILE_MCU_NATIVE_CMD_MODE = 0x01,
    NILE_MCU_NATIVE_CMD_FREQ = 0x02,
    NILE_MCU_NATIVE_CMD_ID = 0x03,
    NILE_MCU_NATIVE_CMD_INFO = 0x04,
    NILE_MCU_NATIVE_CMD_REG_READ = 0x08,
    NILE_MCU_NATIVE_CMD_REG_WRITE = 0x09,
    NILE_MCU_NATIVE_CMD_VERSION = 0x0F,
    NILE_MCU_NATIVE_CMD_EEPROM_MODE = 0x10,
    NILE_MCU_NATIVE_CMD_EEPROM_ERASE = 0x11,
    NILE_MCU_NATIVE_CMD_EEPROM_READ = 0x12,
    NILE_MCU_NATIVE_CMD_EEPROM_WRITE = 0x13,
    NILE_MCU_NATIVE_CMD_RTC_COMMAND = 0x14,
    NILE_MCU_NATIVE_CMD_EEPROM_GET_MODE = 0x15,
    NILE_MCU_NATIVE_CMD_SET_SAVE_ID = 0x16,
    NILE_MCU_NATIVE_CMD_GET_SAVE_ID = 0x17,
    NILE_MCU_NATIVE_CMD_USB_CDC_READ = 0x40,
    NILE_MCU_NATIVE_CMD_USB_CDC_WRITE = 0x41,
    NILE_MCU_NATIVE_CMD_USB_HID_WRITE = 0x42,
    NILE_MCU_NATIVE_CMD_USB_CDC_AVAILABLE = 0x43,
    NILE_MCU_NATIVE_CMD_USB_CDC_FLUSH = 0x44,
    NILE_MCU_NATIVE_CMD_ACCEL_POLL = 0x50,
    NILE_MCU_NATIVE_CMD_ACCEL_READ = 0x51
} nile_mcu_native_cmd_t;

typedef struct __attribute__((packed)) {
    uint16_t status;
    uint16_t caps;
    uint16_t bat_voltage;
} nile_mcu_native_info_t;

typedef enum {
    NILE_MCU_NATIVE_MODE_CMD = 0x00, ///< Native command mode.
    NILE_MCU_NATIVE_MODE_EEPROM = 0x01, ///< EEPROM emulation mode.
    NILE_MCU_NATIVE_MODE_RTC = 0x02, ///< RTC emulation mode.
    NILE_MCU_NATIVE_MODE_CDC = 0x03, ///< CDC output-only mode.
    NILE_MCU_NATIVE_MODE_STANDBY = 0xFF ///< MCU standby mode - will not respond to further SPI messages until reset.
} nile_mcu_native_mode_t;

typedef struct {
    uint16_t major;
    uint16_t minor;
} nile_mcu_native_version_t;

#endif /* __ASSEMBLER__ */

#define NILE_MCU_NATIVE_REG_IRQ_ENABLE         0x000
#define NILE_MCU_NATIVE_REG_IRQ_STATUS         0x001
#define NILE_MCU_NATIVE_REG_IRQ_STATUS_AUTOACK 0x002

#define NILE_MCU_NATIVE_INFO_CAP_EEPROM  0x0001
#define NILE_MCU_NATIVE_INFO_CAP_USB     0x0002
#define NILE_MCU_NATIVE_INFO_CAP_ACCEL   0x0004
#define NILE_MCU_NATIVE_INFO_CAP_RTC     0x0008
#define NILE_MCU_NATIVE_INFO_CAP_BATTERY 0x0010
#define NILE_MCU_NATIVE_INFO_RTC_LSE     0x0001
#define NILE_MCU_NATIVE_INFO_RTC_ENABLED 0x0002
#define NILE_MCU_NATIVE_INFO_USB_DETECT  0x0004
#define NILE_MCU_NATIVE_INFO_USB_CONNECT 0x0008

#endif /* NILE_MCU_PROTOCOL_H_ */
