/*   Copyright (C) 2011 Robert Spanton, Richard Barlow

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */
#ifndef __SRIC_FLASH
#define __SRIC_FLASH
#include <stdint.h>
#include <stdbool.h>
#include "../libsric/sric-client.h"

typedef struct {
	/* A callback to control an LED on the board, it's turned
	 * on when the firmware is being updated */
	void (*led_control) (bool state);
} sric_flash_conf_t;

extern const sric_flash_conf_t sric_flash_conf;

void sric_flash_init( void );

/* Should be called in the main loop of the program. Handles
 * switching to new firmware and writing to the flash asynchronously */
void sric_flash_poll( void );

/* Transmits the firmware version to the master. */
uint8_t sric_flashr_fw_ver(const sric_if_t *iface);

/* Receives a chunk of firmware from the master.
   20 bytes expected. */
uint8_t sric_flashw_fw_chunk(const sric_if_t *iface);

/* Transmits the address of the next required chunk to the master  */
uint8_t sric_flashr_fw_next(const sric_if_t *iface);

/* Transmits the firmware CRC to the master */
uint8_t sric_flashr_crc(const sric_if_t *iface);

/* Receives confirmation from the master that all 
   firmware has been received. 
   4-byte password expected. */
uint8_t sric_flashw_confirm(const sric_if_t *iface);

#endif	/* __SRIC_FLASH */
