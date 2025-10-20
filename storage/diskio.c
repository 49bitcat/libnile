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

#include <string.h>
#include <ws.h>
#include <ws/display.h>
#include <ws/util.h>
#include "nile.h"
#include "ff.h"
#include "nile/hardware.h"
#include "nile/ipc.h"
#include "diskio.h"

#define USE_MULTI_TRANSFER_READS
#define USE_MULTI_TRANSFER_WRITES

#include "tf.h"

// FatFS API implementation

uint8_t card_state;

#ifdef LIBNILE_EXPOSE_DISKIO_DETAIL_CODE
uint8_t diskio_detail_code;
#define set_detail_code(v) diskio_detail_code = v
#else
#define set_detail_code(v)
#endif

/* Wait until the TF card is finished */
/* Returns resp on success, 0xFF on failure */
uint8_t nile_tf_wait_ready(uint8_t resp);
#if 0
uint8_t nile_tf_wait_ready(uint8_t resp) {
	uint32_t timeout = 1500000;
	uint8_t resp_busy[1];
	while (--timeout) {
		// wait for 0xFF to signify end of busy time
		if (!nile_spi_rx_sync_block(&resp_busy, 1, NILE_SPI_MODE_READ))
			return 0xFF;
		if (resp_busy[0] == 0xFF)
			break;
	}

	if (!timeout)
		return 0xFF;
	return resp;
}
#endif

bool nile_tf_cs_high(void);
#if 0
bool nile_tf_cs_high(void) {
	if (!nile_spi_wait_ready())
		return false;
	uint16_t spi_cnt = inportw(IO_NILE_SPI_CNT);
	spi_cnt &= ~NILE_SPI_DEV_MASK;
	spi_cnt |= NILE_SPI_DEV_NONE;
	outportw(IO_NILE_SPI_CNT, spi_cnt);
	if (!nile_spi_rx_async(1, NILE_SPI_MODE_READ))
		return false;
	return true;
}
#endif

bool nile_tf_cs_low(void);
#if 0
bool nile_tf_cs_low(void) {
	if (!nile_spi_wait_ready())
		return false;
	uint16_t spi_cnt = inportw(IO_NILE_SPI_CNT);
	spi_cnt &= ~NILE_SPI_DEV_MASK;
	spi_cnt |= NILE_SPI_DEV_TF;
	outportw(IO_NILE_SPI_CNT, spi_cnt);
	if (!nile_spi_rx_async(1, NILE_SPI_MODE_READ))
		return false;
	if (nile_tf_wait_ready(0x00))
		return false;
	return true;
}
#endif

static uint8_t nile_tf_read_response_r1b(void) {
	uint8_t resp = 0xFF;

	if (!nile_spi_rx_sync_block(&resp, 1, NILE_SPI_MODE_WAIT_READ) || resp)
		return resp;

	return nile_tf_wait_ready(resp);
}

static uint8_t nile_tf_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t *recv_buffer, uint16_t size) {
	uint8_t cmd_buffer[6];
	recv_buffer[0] = 0xFF;

	if (cmd & 0x80) {
		uint8_t resp = nile_tf_command(TFC_APP_PREFIX, 0x95, 0, recv_buffer, 1);
		if (resp & ~TFC_R1_IDLE)
			return resp;
	}

	if (!nile_tf_cs_high())
		return 0xFF;
	if (!nile_tf_cs_low())
		return 0xFF;

	cmd_buffer[0] = cmd & 0x7F;
	cmd_buffer[1] = arg >> 24;
	cmd_buffer[2] = arg >> 16;
	cmd_buffer[3] = arg >> 8;
	cmd_buffer[4] = arg;
	cmd_buffer[5] = crc;
	if (!nile_spi_tx_async_block(cmd_buffer, sizeof(cmd_buffer)))
		return 0xFF;
	if (!nile_spi_rx_sync_block(recv_buffer, size + 1, NILE_SPI_MODE_WAIT_READ))
		return 0xFF;
	return recv_buffer[0];
}

DSTATUS disk_status(BYTE pdrv) {
	return card_state == 0 ? STA_NOINIT : 0;
}

#define MAX_RETRIES 1000

void nilefs_ipc_sync(void) {
	uint16_t prev_sram_bank = inportw(WS_CART_EXTBANK_RAM_PORT);
	outportw(WS_CART_EXTBANK_RAM_PORT, NILE_SEG_RAM_IPC);
	card_state = MEM_NILE_IPC->tf_card_status;
	outportw(WS_CART_EXTBANK_RAM_PORT, prev_sram_bank);
}

void nilefs_eject(void) {
	// Set card status to disabled
	uint16_t prev_sram_bank = inportw(WS_CART_EXTBANK_RAM_PORT);
	outportw(WS_CART_EXTBANK_RAM_PORT, NILE_SEG_RAM_IPC);
	card_state = 0;
	MEM_NILE_IPC->tf_card_status = 0;
	outportw(WS_CART_EXTBANK_RAM_PORT, prev_sram_bank);

	// Disable TF card power
	outportb(IO_NILE_POW_CNT, inportb(IO_NILE_POW_CNT) & ~NILE_POW_TF);
}

DSTATUS disk_initialize(BYTE pdrv) {
	uint16_t retries;
	uint8_t buffer[8];

	nilefs_ipc_sync();
	if (card_state != 0) return 0;

	card_state = 0;
	nile_spi_set_timeout(1000);

	if (!nile_spi_wait_ready())
		return STA_NOINIT;

	set_detail_code(0);
	uint16_t uses_fast_clock = inportw(IO_NILE_SPI_CNT) & NILE_SPI_CLOCK_CART;
	outportw(IO_NILE_SPI_CNT, NILE_SPI_DEV_NONE | NILE_SPI_CLOCK_CART);
	nile_tf_cs_high();

	uint8_t powcnt = inportb(IO_NILE_POW_CNT);
	if (!(powcnt & NILE_POW_TF)) {
		// Power card on
		powcnt |= NILE_POW_TF;
		outportb(IO_NILE_POW_CNT, powcnt);
	
		ws_delay_ms(250);
	}

	nile_spi_rx_async(10, NILE_SPI_MODE_READ);

	// Reset card
	if (nile_tf_command(TFC_GO_IDLE_STATE, 0, 0x95, buffer, 1) & ~TFC_R1_IDLE) {
		// Error/No response
		set_detail_code(1);
		goto card_init_failed;
	}

	// Query interface configuration
	if (!(nile_tf_command(TFC_SEND_IF_COND, 0x000001AA, 0x87, buffer, 5) & ~TFC_R1_IDLE)) {
		// Check voltage/pattern value match
		if ((buffer[3] & 0xF) == 0x1 && buffer[4] == 0xAA) {
			// Attempt high-capacity card init
			retries = MAX_RETRIES;
			nile_spi_set_timeout(10);
			while (--retries) {
				uint8_t init_response = nile_tf_command(TFC_APP_SEND_OP_COND, 1UL << 30, 0x95, buffer, 1);
				if (init_response & ~TFC_R1_IDLE) {
					// Initialization error
					retries = 0;
					break;
				} else if (!init_response) {
					// Initialization success
					card_state = NILE_IPC_TF_TYPE_TF_NEW;
					break;
				}
				// Card still idle, try again
				ws_delay_ms(1);
			}

			// Card init successful?
			if (card_state) {
				// Read OCR to check for HC card
				if (!nile_tf_command(TFC_READ_OCR, 0, 0x95, buffer, 5)) {
					if (buffer[1] & 0x40) {
						card_state = NILE_IPC_TF_BLOCK | NILE_IPC_TF_TYPE_TF_NEW;
					}
				}
				goto card_init_complete_hc;
			}
		} else {
			// Voltage/pattern value mismatch
			set_detail_code(2);
			goto card_init_failed;
		}
	}

	// Attempt card init
	retries = MAX_RETRIES;
	nile_spi_set_timeout(10);
	uint8_t init_command = TFC_APP_SEND_OP_COND;
	while (--retries) {
		uint8_t init_response = nile_tf_command(init_command, 0, 0x95, buffer, 1);
		if (init_response & ~TFC_R1_IDLE) {
			// Initialization error
			if (init_command == TFC_APP_SEND_OP_COND) {
				// Try legacy card init command next
				init_command = TFC_SEND_OP_COND;
			} else {
				retries = 0;
				break;
			}
		} else if (!init_response) {
			// Initialization success
			if (init_command == TFC_APP_SEND_OP_COND) {
				card_state = NILE_IPC_TF_TYPE_TF_OLD;
			} else {
				card_state = NILE_IPC_TF_TYPE_MMC_OLD;
			}
			goto card_init_complete;
		}
		// Card still idle, try again
		ws_delay_ms(1);
	}

	set_detail_code(3);
card_init_failed:
	// Power off card
	outportb(IO_NILE_POW_CNT, powcnt & ~NILE_POW_TF);
	outportw(IO_NILE_SPI_CNT, 0);
	return STA_NOINIT;

card_init_complete:
card_init_complete_hc:
	nile_spi_set_timeout(100);
	// Set block size to 512
	if (nile_tf_command(TFC_SET_BLOCKLEN, 512, 0x95, buffer, 1)) {
		set_detail_code(4);
		goto card_init_failed;
	}

	{
		uint16_t prev_sram_bank = inportw(WS_CART_EXTBANK_RAM_PORT);
		outportw(WS_CART_EXTBANK_RAM_PORT, NILE_SEG_RAM_IPC);
		MEM_NILE_IPC->tf_card_status = card_state;
		outportw(WS_CART_EXTBANK_RAM_PORT, prev_sram_bank);
	}

	// nile_tf_cs_high(); but also changes clocks
	if (!nile_spi_wait_ready())
		return 0;
	outportb(IO_NILE_POW_CNT, powcnt | NILE_POW_CLOCK);
	outportw(IO_NILE_SPI_CNT, NILE_SPI_DEV_NONE | uses_fast_clock);
	if (!nile_spi_rx_async(1, NILE_SPI_MODE_READ))
		return 0;
	return 0;
}

bool nile_disk_read_inner(BYTE __far* buff, uint16_t count);

__attribute__((noinline))
static bool nile_tf_read_data(void __far* buff, uint16_t len) {
	uint8_t resp[1];
	
	if (!nile_spi_rx_sync_block(resp, 1, NILE_SPI_MODE_WAIT_READ)) {
		set_detail_code(0x11);
		return false;
	}
	if (resp[0] != 0xFE) {
		set_detail_code(0xE0 | resp[0]);
		return false;
	}
	if (!nile_spi_rx_sync_block(buff, len, NILE_SPI_MODE_READ)) {
		set_detail_code(0x13);
		return false;
	}
	if (!nile_spi_rx_async(2, NILE_SPI_MODE_READ)) {
		set_detail_code(0x14);
		return false;
	}
	return true;
}

DRESULT disk_read (BYTE pdrv, BYTE FF_WF_DATA_BUFFER_ADDRESS_SPACE* buff, LBA_t sector, UINT count) {
	uint8_t result = RES_ERROR;
	uint8_t resp[8];

	if (!(card_state & NILE_IPC_TF_BLOCK))
		sector <<= 9;

#ifdef USE_MULTI_TRANSFER_READS
	bool multi_transfer = count > 1;
	if (nile_tf_command(multi_transfer ? TFC_READ_MULTIPLE_BLOCK : TFC_READ_SINGLE_BLOCK, sector, 0x95, resp, 0)) {
		set_detail_code(0x10);
		goto disk_read_end;
	}

#if 1
	if (!nile_disk_read_inner(buff, count)) 
		goto disk_read_stop;
#else
	while (count) {
		if (!nile_spi_rx_sync_block(resp, 1, NILE_SPI_MODE_WAIT_READ)) {
			set_detail_code(0x11);
			goto disk_read_stop;
		}
		if (resp[0] != 0xFE) {
			set_detail_code(0xE0 | resp[0]);
			goto disk_read_stop;
		}
		if (!nile_spi_rx_sync_block(buff, 512, NILE_SPI_MODE_READ)) {
			set_detail_code(0x13);
			goto disk_read_stop;
		}
		if (!nile_spi_rx_async(2, NILE_SPI_MODE_READ)) {
			set_detail_code(0x14);
			goto disk_read_stop;
		}
		buff += 512;
		count--;
	}
#endif
	result = RES_OK;

disk_read_stop:
	if (multi_transfer) {
		resp[0] = TFC_STOP_TRANSMISSION;
		resp[5] = 0x95;
		resp[6] = 0xFF; // skip one byte
		if (!nile_spi_tx_async_block(resp, 7)) {
			set_detail_code(0x15);
			result = RES_ERROR;
			goto disk_read_end;
		}
		if (nile_tf_read_response_r1b()) {
			set_detail_code(0x16);
			result = RES_ERROR;
			goto disk_read_end;
		}
	}
#else
	while (count) {
		if (nile_tf_command(TFC_READ_SINGLE_BLOCK, sector, 0x95, resp, 0)) {
			set_detail_code(0x10);
			goto disk_read_end;
		}

		if (!nile_tf_read_data(buff, 512)) {
			goto disk_read_end;
		}
		buff += 512;
		sector += (card_state & NILE_IPC_TF_BLOCK) ? 1 : 512;
		count--;
	}
#endif

	result = RES_OK;
disk_read_end:
	nile_tf_cs_high();
	return result;
}

#if FF_FS_READONLY == 0

DRESULT disk_write (BYTE pdrv, const BYTE FF_WF_DATA_BUFFER_ADDRESS_SPACE* buff, LBA_t sector, UINT count) {
	uint8_t result = RES_ERROR;
	uint8_t resp[2];

	if (!(card_state & NILE_IPC_TF_BLOCK))
		sector <<= 9;

#ifdef USE_MULTI_TRANSFER_WRITES
	bool multi_transfer = count > 1;
	if (multi_transfer) {
		if ((card_state & NILE_IPC_TF_TYPE_TF) && nile_tf_command(TFC_SET_BLOCK_COUNT, count, 0x95, resp, 1)) {
			set_detail_code(0x29);
			goto disk_read_end;
		}
		if (nile_tf_command(TFC_WRITE_MULTIPLE_BLOCK, sector, 0x95, resp, 1)) {
			set_detail_code(0x20);
			goto disk_read_end;
		}
	} else {
		if (nile_tf_command(TFC_WRITE_BLOCK, sector, 0x95, resp, 1)) {
			set_detail_code(0x20);
			goto disk_read_end;
		}
	}

	while (count) {
		resp[0] = 0xFF;
		resp[1] = multi_transfer ? 0xFC : 0xFE;
		if (!nile_spi_tx_async_block(resp, 2)) {
			set_detail_code(0x21);
			goto disk_read_stop;
		}
		if (!nile_spi_tx_async_block(buff, 512)) {
			set_detail_code(0x22);
			goto disk_read_stop;
		}
		resp[1] = 0xFF;
		if (!nile_spi_tx_async_block(resp, 2)) {
			set_detail_code(0x23);
			goto disk_read_stop;
		}
		if (!nile_spi_rx_sync_block(resp, 1, NILE_SPI_MODE_READ)) {
			set_detail_code(0x24);
			goto disk_read_stop;
		}
		if ((resp[0] & 0x1F) != 0x05) {
			set_detail_code(0x25);
			goto disk_read_end;
		}
		if (nile_tf_wait_ready(0x00)) {
			set_detail_code(0x26);
			goto disk_read_end;
		}
		buff += 512;
		count--;
	}
disk_read_stop:

	if (multi_transfer) {
		resp[0] = 0xFD;
		resp[1] = 0xFF;
		if (!nile_spi_tx_async_block(resp, 2)) {
			set_detail_code(0x27);
			goto disk_read_stop;
		}
		if (nile_tf_wait_ready(0x00)) {
			set_detail_code(0x28);
			goto disk_read_stop;
		}
	}
#else
	while (count) {
		if (nile_tf_command(TFC_WRITE_BLOCK, sector, 0x95, resp, 1)) {
			set_detail_code(0x20);
			goto disk_read_end;
		}

		resp[0] = 0xFF;
		resp[1] = 0xFE;
		if (!nile_spi_tx_async_block(resp, 2)) {
			set_detail_code(0x21);
			goto disk_read_end;
		}
		if (!nile_spi_tx_async_block(buff, 512)) {
			set_detail_code(0x22);
			goto disk_read_end;
		}
		if (!nile_spi_rx_sync_block(resp, 2, NILE_SPI_MODE_WAIT_READ)) {
			set_detail_code(0x23);
			goto disk_read_end;
		}
		if ((resp[0] & 0x1F) != 0x05) {
			set_detail_code(0x25);
			goto disk_read_end;
		}
		buff += 512;
		sector += (card_state & NILE_IPC_TF_BLOCK) ? 1 : 512;
		count--;
	}
#endif

	result = RES_OK;
disk_read_end:
	nile_tf_cs_high();
	return result;
}

#endif

bool nilefs_read_card_csd(void __far* buff) {
	uint8_t resp[2];
	bool result = !nile_tf_command(TFC_SEND_CSD, 0, 0x95, resp, 1) && nile_tf_read_data(buff, 16);
	nile_tf_cs_high();
	return result;
}

bool nilefs_read_card_cid(void __far* buff) {
	uint8_t resp[2];
	bool result = !nile_tf_command(TFC_SEND_CID, 0, 0x95, resp, 1) && nile_tf_read_data(buff, 16);
	nile_tf_cs_high();
	return result;
}

bool nilefs_read_card_ssr(void __far* buff) {
	uint8_t resp[2];
	bool result = !nile_tf_command(TFC_SEND_SSR, 0, 0x95, resp, 1) && nile_tf_read_data(buff, 64);
	nile_tf_cs_high();
	return result;
}

uint32_t nilefs_read_card_sector_count(void) {
	uint8_t csd[16];
	
	if (nilefs_read_card_csd(csd)) {
		switch (csd[0] >> 6) {
		case 0: {
			uint16_t block_size = 1 << (csd[5] & 0xF);
			uint8_t size_mul_shift = ((csd[10] >> 7) | (csd[9] << 1)) & 0x7;
			uint16_t size_mul = 1 << (size_mul_shift + 2);
			uint16_t size = ((csd[8] >> 6) | (csd[7] << 2) | ((csd[6] & 0x3) << 10)) + 1;
			return ((uint32_t) size * size_mul * block_size) >> 9;
		}
		case 1: {
			return ((csd[9] | (csd[8] << 8) | ((uint32_t) csd[7] << 16)) + 1L) << 10;
		}
		}
	}
	return 0;
}

static const uint8_t ws_rom alloc_unit_table[5] = {12, 16, 24, 32, 64};

uint32_t nilefs_read_card_block_size(void) {
	uint8_t csd[64];
	
	if (card_state & NILE_IPC_TF_TYPE_TF_NEW) {
		if (nilefs_read_card_ssr(csd)) {
			uint8_t shift = csd[10] >> 4;
			if (shift >= 11)
				return alloc_unit_table[shift - 11] * 2048UL;
			return 16UL << shift;
		}
	} else {
		if (nilefs_read_card_csd(csd)) {
			if (card_state & NILE_IPC_TF_TYPE_MMC) {
				uint8_t size = ((csd[10] >> 2) & 0x1F) + 1;
				uint8_t mul = ((csd[10] & 0x3) << 3) + (csd[11] >> 5) + 1;
				return size * mul;
			} else {
				return (((csd[10] & 0x3F) << 1) | (csd[11] >> 7)) + 1;
			}
		}
	}

	return 0;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff) {
	uint32_t v;

#ifndef LIBNILE_IPL1
	switch (cmd) {
		case CTRL_SYNC:
			return RES_OK;
		case GET_BLOCK_SIZE:
			v = nilefs_read_card_block_size();
			*((DWORD*) buff) = v;
			return v ? RES_OK : RES_ERROR;
		case GET_SECTOR_COUNT:
			v = nilefs_read_card_sector_count();
			*((DWORD*) buff) = v;
			return v ? RES_OK : RES_ERROR;
	}
#endif

	return RES_PARERR;
}
