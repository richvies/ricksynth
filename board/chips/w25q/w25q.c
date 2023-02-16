/****************************************************************************

RickSynth
----------

MIT License

Copyright (c) [2021] [Richard Davies]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/


#include "config_board.h"


#ifdef W25Q_NUM_OF


#include "w25q.h"
#include "io.h"


typedef struct
{
	W25Q_id_e id;
	uint32_t 	sector_count;
} W25Q_t;


#define CMD_WRITE_ENABLE			(0x06)
#define CMD_WRITE_DISABLE			(0x04)
#define CMD_ERASE_SECTOR			(0x20)
#define CMD_PAGE_PROGRAM			(0x02)
#define CMD_READ							(0x03)

#define DUMMY_BYTE 0xA5

#define STATUS_REG_1_BUSY_FLAG	(1 << 0)


#define TX(d, l) 				({ SPI_writeBlocking(W25Q_SPI_CH, W25Q_CS_PIN, d, l); })
#define RX(d, l) 				({ SPI_readBlocking(W25Q_SPI_CH, W25Q_CS_PIN, d, l); })
#define TXRX(td, rd, l) ({ SPI_writeReadBlocking(W25Q_SPI_CH, W25Q_CS_PIN, td, rd, l); })


static W25Q_t w25q;
static uint8_t spi_tx_buf[16];
static uint8_t spi_rx_buf[16];


static bool 		writeEnable(void);
static uint8_t  readCapacity(void);
static uint8_t 	readStatusReg(uint8_t num);
static bool 		waitForWriteEnd(void);


bool W25Q_init(void)
{
	SPI_cfg_t cfg;
	uint32_t block_count;

	/* try to start spi peripheral */
	cfg.cs_pin = W25Q_CS_PIN;
	cfg.master_mode = SPI_MASTER_MODE;
	cfg.clk_speed_hz = 100000;
	cfg.bit_order = SPI_BIT_ORDER_MSB_FIRST;
	cfg.clk_phase = SPI_CLK_PHASE_SAMPLE_FIRST_EDGE;
	cfg.clk_polarity = SPI_CLK_POLARITY_IDLE_LOW;
	if (false == SPI_init(W25Q_SPI_CH, &cfg)) {
		return false; }

	switch (readCapacity())
	{
	case 0x18:
		w25q.id = W25Q128;
		block_count = 256;
		break;
	case 0x17:
		w25q.id = W25Q64;
		block_count = 128;
		break;
	case 0x16:
		w25q.id = W25Q32;
		block_count = 64;
		break;
	case 0x15:
		w25q.id = W25Q16;
		block_count = 32;
		break;
	case 0x14:
		w25q.id = W25Q80;
		block_count = 16;
		break;
	case 0x13:
		w25q.id = W25Q40;
		block_count = 8;
		break;
	case 0x12:
		w25q.id = W25Q20;
		block_count = 4;
		break;
	case 0x11:
		w25q.id = W25Q10;
		block_count = 2;
		break;
	default:
		return false;
		break;
	}

	w25q.sector_count = block_count * 16;

	return true;
}


bool W25Q_eraseSector(uint32_t sector)
{
	bool ret = false;
	uint8_t i = 0;
	uint32_t addr;

	if (false == writeEnable()) {
		return false; }

	addr = sector * W25Q_SECTOR_SIZE;

	spi_tx_buf[i++] = CMD_ERASE_SECTOR;
	spi_tx_buf[i++] = (addr & 0xFF0000) >> 16;
	spi_tx_buf[i++] = (addr & 0xFF00) >> 8;
	spi_tx_buf[i++] = addr & 0xFF;

	if ((true == TX(spi_tx_buf, i))
	&&  (true == waitForWriteEnd()))
	{
		ret = true;
	}

	return ret;
}

bool W25Q_programSector(uint32_t sector,
											  uint32_t offset,
												void *data,
												uint32_t len)
{
	uint32_t addr = 0;
	uint32_t prog_size = W25Q_PROG_SIZE - offset;
	bool ret = true;

	if (false == writeEnable()) {
		return false; }

	addr = (sector * W25Q_SECTOR_SIZE) + offset;

	spi_tx_buf[0] = CMD_PAGE_PROGRAM;

	while (len)
	{
		spi_tx_buf[1] = (addr & 0xFF0000) >> 16;
		spi_tx_buf[2] = (addr & 0xFF00) >> 8;
		spi_tx_buf[3]  = addr & 0xFF;

		if (prog_size > len) {
			prog_size = len; }

		IO_clear(W25Q_CS_PIN);
		if ((false == SPI_writeBlocking(W25Q_SPI_CH, SPI_NO_CS_PIN, spi_tx_buf, 4))
		||  (false == SPI_writeBlocking(W25Q_SPI_CH, SPI_NO_CS_PIN, data, prog_size))
		||  (false == waitForWriteEnd()))
		{
			ret = false;
			break;
		}
		IO_set(W25Q_CS_PIN);

		data += prog_size;
		len  -= prog_size;
		addr += prog_size;
		prog_size = W25Q_PROG_SIZE;
	}

	return ret;
}

bool W25Q_readSector(uint32_t sector, uint32_t offset, void *data, uint32_t len)
{
	uint8_t i = 0;
	uint32_t addr;
	bool ret = false;

	if (false == writeEnable()) {
		return false; }

	addr = (sector * W25Q_SECTOR_SIZE) + offset;

	spi_tx_buf[i++] = CMD_READ;
	spi_tx_buf[i++] = (addr & 0xFF0000) >> 16;
	spi_tx_buf[i++] = (addr & 0xFF00) >> 8;
	spi_tx_buf[i++] = addr & 0xFF;

	IO_clear(W25Q_CS_PIN);
	if ((true == SPI_writeBlocking(W25Q_SPI_CH, SPI_NO_CS_PIN, spi_tx_buf, i))
	&&  (true == SPI_readBlocking(W25Q_SPI_CH, SPI_NO_CS_PIN, data, len))
	&&  (true == waitForWriteEnd()))
	{
		ret = true;
	}
	IO_set(W25Q_CS_PIN);

	return ret;
}


static bool	writeEnable(void)
{
	bool ret = false;
	uint8_t i = 0;

	spi_tx_buf[i++] = CMD_WRITE_ENABLE;

	ret = TX(spi_tx_buf, i);

	return ret;
}

static uint8_t readCapacity(void)
{
	uint8_t i = 0;
	uint8_t len = 4;
	uint8_t capacity = 0;

	spi_tx_buf[i++] = 0x9F;
	memset(spi_tx_buf + i, DUMMY_BYTE, len - i);

	if (true == TXRX(spi_tx_buf, spi_rx_buf, len))
	{
		capacity = spi_rx_buf[3];
	}

	return capacity;
}

static uint8_t readStatusReg(uint8_t num)
{
	uint8_t i = 0;
	uint8_t reg = 0;

	if (num == 1)
	{
		spi_tx_buf[i++] = 0x05;
	}
	else if (num == 2)
	{
		spi_tx_buf[i++] = 0x35;
	}
	else
	{
		spi_tx_buf[i++] = 0x15;
	}

	spi_tx_buf[i++] = DUMMY_BYTE;

	if (true == TXRX(spi_tx_buf, spi_rx_buf, 2))
	{
		reg = spi_rx_buf[1];
	}

	return reg;
}

static bool waitForWriteEnd(void)
{
	bool ret;

	TIMEOUT(readStatusReg(1) & STATUS_REG_1_BUSY_FLAG,
					100,
					EMPTY,
					ret = true,
					ret = false);

	return ret;
}


#endif
