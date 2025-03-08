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

#ifndef __NILE_SPI_H__
#define __NILE_SPI_H__

#include <wonderful.h>
#include "hardware.h"

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Get the SPI communication timeout value, in milliseconds.
 */
uint16_t nile_spi_get_timeout(void);

/**
 * @brief Set the SPI communication timeout value, in milliseconds.
 */
void nile_spi_set_timeout(uint16_t ms);

/**
 * @brief Wait until the SPI interface is ready.
 * The maximum number of milliseconds for the wait is set using
 * nile_spi_set_timeout.
 *
 * @return true The SPI interface is now ready.
 * @return false Wait timed out.
 */
bool nile_spi_wait_ready(void);

/**
 * @brief Abort SPI transfer.
 *
 * @return true The SPI interface is now ready.
 * @return false Wait timed out.
 */
bool nile_spi_abort(void);

/**
 * @brief Safely set the control port.
 */
bool nile_spi_set_control(uint16_t value);

/**
 * @brief Flip the currently accessible SPI buffer.
 */
static inline void nile_spi_buffer_flip(void) {
    outportb(IO_NILE_SPI_CNT + 1, inportb(IO_NILE_SPI_CNT + 1) ^ (NILE_SPI_BUFFER_IDX >> 8));
}

/**
 * @brief Asynchronously receive block of data over the SPI interface.
 * 
 * @param size Length of data to receive.
 * @param mode Transfer mode.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
bool nile_spi_rx_async(uint16_t size, uint16_t mode);

/**
 * @brief Asynchronously transfer block of data over the SPI interface.
 * 
 * @param size Length of data block.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
static inline bool nile_spi_tx_async(uint16_t size) {
    return nile_spi_rx_async(size, NILE_SPI_MODE_WRITE);
}

/**
 * @brief Asynchronously transfer block of data over the SPI interface.
 * 
 * @param buf Data block.
 * @param size Length of data block.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
bool nile_spi_tx_async_block(const void __far* buf, uint16_t size);

/**
 * @brief Synchronously transfer block of data over the SPI interface.
 * 
 * @param buf Data block.
 * @param size Length of data block.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
static inline bool nile_spi_tx_sync_block(const void __far* buf, uint16_t size) {
    return nile_spi_tx_async_block(buf, size) && nile_spi_wait_ready();
}

/**
 * @brief Synchronously receive block of data over the SPI interface
 * and flip the buffer so that it can be accessed.
 * 
 * @param size Length of data to receive.
 * @param mode Transfer mode.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
bool nile_spi_rx_sync_flip(uint16_t size, uint16_t mode);

/**
 * @brief Synchronously receive block of data over the SPI interface.
 * 
 * @param buf Destination data block.
 * @param size Length of data to receive.
 * @param mode Transfer mode.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
bool nile_spi_rx_sync_block(void __far* buf, uint16_t size, uint16_t mode);

/**
 * @brief Synchronously exchange block of data over the SPI interface.
 * 
 * @param buf Destination data block.
 * @param size Length of data to send and receive.
 * @param mode Transfer mode.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
bool nile_spi_xch_block(void __far* buf, uint16_t size);

/**
 * @brief Synchronously exchange byte over the SPI interface.
 * 
 * @param value Byte to exchange.
 * @return true Transfer successful.
 * @return false Transfer failed.
 */
uint16_t nile_spi_xch(uint8_t value);
#define NILE_SPI_XCH_ERROR_MASK 0xFF00

#endif /* __ASSEMBLER__ */

#endif /* __NILE_SPI_H__ */
