#include <stddef.h>
#include "sric-flash.h"
#include "flash.h"

#define FLASH_MAGIC 0x6452

volatile uint16_t sric_flash_received = 0;

#define flash_received() ( sric_flash_received == FLASH_MAGIC )
#define repeat(x) do { x; x; x; x; x; } while(0)

void sric_flash_init( void )
{
	sric_flash_received = 0;
}

void sric_flash_poll( void )
{
	/* Check that we've received the flash 25 times */
	repeat( repeat( if( !flash_received() ) return ) );

	/* Clear the magic number so that the new firmware has
	 no chance of believing it has received new firmware */
	sric_flash_received = 0;

	flash_switchover();
}

/* Transmits the firmware version to the master. */
uint8_t sric_flashr_fw_ver(const sric_if_t *iface)
{
	/* Reinitialise the flashing system */
	flash_init();

	uint8_t *buf = iface->txbuf + SRIC_DATA;

	buf[0] = firmware_version & 0xff;
	buf[1] = (firmware_version>>8) &0xff;

	return 2;
}

/* Receives a chunk of firmware from the master.
   20 bytes expected. */
uint8_t sric_flashw_fw_chunk(const sric_if_t *iface)
{
	uint8_t *buf = iface->rxbuf + SRIC_DATA+1;
	uint16_t ver, addr;
	/* Format:
	    0-1: Firmware version (0 is lsb)
	    2-3: Address (2 is lsb)
	   4-19: The data */
	ver = buf[0] | (((uint16_t)buf[1]) << 8);
	addr = buf[2] | (((uint16_t)buf[3]) << 8);

	if (sric_flash_conf.led_control != NULL)
		sric_flash_conf.led_control(true);

	flash_rx_chunk( addr, buf + 4 );
	return 0;
}

/* Transmits the address of the next required chunk to the master  */
uint8_t sric_flashr_fw_next(const sric_if_t *iface)
{
	uint8_t *buf = iface->txbuf + SRIC_DATA;

	buf[0] = ((uint16_t)next_chunk) & 0xff;
	buf[1] = ( ((uint16_t)next_chunk) >> 8 ) & 0xff;

	return 2;
}

/* Transmits the firmware CRC to the master */
uint8_t sric_flashr_crc(const sric_if_t *iface)
{
	uint8_t *buf = iface->txbuf + SRIC_DATA;

	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	return 4;
}

/* Receives confirmation from the master that all 
   firmware has been received. 
   4-byte password expected. */
uint8_t sric_flashw_confirm(const sric_if_t *iface)
{
	/* TODO: Do the CRC */
	sric_flash_received = FLASH_MAGIC;
	return 0;
}
