#include "w25q.h"


#define W25Q_DUMMY_BYTE 0xA5


#define  (d, l) 				({ bool r; xfer.dir = SPI_WRITE; xfer.tx_data = d; xfer.length = l; r = SPI_xferBlocking(spi_ch, &xfer); r; })
#define RX(d, l) 				({ bool r; xfer.dir = SPI_READ; xfer.rx_data = d; xfer.length = l; r = SPI_xferBlocking(spi_ch, &xfer); r; })
#define TXRX(td, rd, l) ({ bool r; xfer.dir = SPI_WRITE_READ; xfer.tx_data = td; xfer.rx_data = rd; xfer.length = l; r = SPI_xferBlocking(spi_ch, &xfer); r; })


extern W25Q_t w25q;

static SPI_ch_e spi_ch;
static SPI_xfer_info_t xfer;

static uint8_t buf[16];


static bool 		readId(void);
static bool 		readUniqueId(void);
static bool 		writeEnable(void);
static bool 		writeDisable(void);
static uint8_t 	readStatusRegister(uint8_t num);
static bool 		writeStatusRegister(uint8_t num, uint8_t data);
static bool 		waitForWriteEnd(void);


bool W25Q_init(SPI_ch_e ch, IO_num_e cs_pin)
{
	uint16_t id;
	SPI_cfg_t cfg;
	bool ret = false;

	spi_ch = ch;
	xfer.cs_pin = cs_pin;

	/* try to start spi peripheral */
	cfg.master_mode = SPI_MASTER_MODE;
	cfg.clk_speed_hz = 1000000;
	cfg.bit_order = SPI_BIT_ORDER_MSB_FIRST;
	cfg.clk_phase = SPI_CLK_PHASE_SAMPLE_FIRST_EDGE;
	cfg.clk_polarity = SPI_CLK_POLARITY_IDLE_LOW;
	if (false == SPI_init(spi_ch, &cfg))
		return false;

	readId();

	switch (id & 0x000000FF)
	{
	case 0x20:
		w25q.id = W25Q512;
		w25q.block_count = 1024;
		break;
	case 0x19:
		w25q.id = W25Q256;
		w25q.block_count = 512;
		break;
	case 0x18:
		w25q.id = W25Q128;
		w25q.block_count = 256;
		break;
	case 0x17:
		w25q.id = W25Q64;
		w25q.block_count = 128;
		break;
	case 0x16:
		w25q.id = W25Q32;
		w25q.block_count = 64;
		break;
	case 0x15:
		w25q.id = W25Q16;
		w25q.block_count = 32;
		break;
	case 0x14:
		w25q.id = W25Q80;
		w25q.block_count = 16;
		break;
	case 0x13:
		w25q.id = W25Q40;
		w25q.block_count = 8;
		break;
	case 0x12:
		w25q.id = W25Q20;
		w25q.block_count = 4;
		break;
	case 0x11:
		w25q.id = W25Q10;
		w25q.block_count = 2;
		break;
	default:
		return false;
		break;
	}

	w25q.page_size = 256;
	w25q.sector_size = 0x1000;
	w25q.sector_count = w25q.block_count * 16;
	w25q.page_count = (w25q.sector_count * w25q.sector_size) / w25q.page_size;
	w25q.block_size = w25q.sector_size * 16;
	w25q.size_in_kbytes = (w25q.sector_count * w25q.sector_size) / 1024;

	readUniqueId();
	readStatusRegister(1);
	readStatusRegister(2);
	readStatusRegister(3);

	return true;
}


bool W25Q_eraseBlock(uint32_t block)
{
	bool ret = false;
	uint8_t i = 0;

	addr *= w25q.sector_size;
	waitForWriteEnd();

	writeEnable();

	if (w25q.id >= W25Q256)
	{
		buf[i++] = 0x21;
		buf[i++] = (addr & 0xFF000000) >> 24;
	}
	else
	{
		buf[i++] = 0x20;
	}
	buf[i++] = (addr & 0xFF0000) >> 16;
	buf[i++] = (addr & 0xFF00) >> 8;
	buf[i++] = addr & 0xFF;

	ret = TX(buf, i);

	waitForWriteEnd();

	return ret;
}

bool W25Q_program(uint32_t block, uint32_t offset, uint8_t *data, uint32_t len)
{
	uint8_t i = 0;

	waitForWriteEnd();
	writeEnable();

	buf[0] = 0x06;
	ret = TX(buf, 1);

	if (w25q.id >= W25Q256)
	{
		buf[i++] = 0x12;
		buf[i++] = 0x12;
		buf[i++] = (addr & 0xFF000000) >> 24;
	}
	else
	{
		buf[i++] = 0x02;
	}
	buf[i++] = (addr & 0xFF0000) >> 16;
	buf[i++] = (addr & 0xFF00) >> 8;
	buf[i++] = addr & 0xFF;
	buf[i++] = data;
	waitForWriteEnd();
}

bool W25Q_read(uint32_t block, uint32_t offset, uint8_t *data, uint32_t len)
{

	if (w25q.id >= W25Q256)
	{
		buf[i++] = 0x0C;
		buf[i++] = (addr & 0xFF000000) >> 24;
	}
	else
	{
		buf[i++] = 0x0B;
	}
	buf[i++] = (addr & 0xFF0000) >> 16;
	buf[i++] = (addr & 0xFF00) >> 8;
	buf[i++] = addr & 0xFF;
	buf[i++] = 0;
	HAL_SPI_Receive(&_W25Q_SPI, data, len, 2000);
}


static bool readId(void)
{
	bool ret;

	buf[0] = 0x9F;
	ret = TXRX(buf, buf + 1, 4);
	w25q.id = buf[3];

	return ret;
}

static bool readUniqueId(void)
{
	bool ret;

	buf[0] = 0x4B;
	ret = TXRX(buf, buf + 1, 13);

	memcpy(&w25q.uniqe_id[0], &buf[5], 8);

	return ret;
}

static uint8_t readStatusRegister(uint8_t num)
{
	uint8_t status = 0;
	if (num == 1)
	{
		buf[i++] = 0x05;
		status = buf[i++] = W25Q_DUMMY_BYTE;
		w25q.status_reg_1 = status;
	}
	else if (num == 2)
	{
		buf[i++] = 0x35;
		status = buf[i++] = W25Q_DUMMY_BYTE;
		w25q.status_reg_2 = status;
	}
	else
	{
		buf[i++] = 0x15;
		status = buf[i++] = W25Q_DUMMY_BYTE;
		w25q.status_reg_3 = status;
	}
	return status;
}

static bool writeStatusRegister(uint8_t num, uint8_t data)
{
	if (num == 1)
	{
		buf[i++] = 0x01;
		w25q.status_reg_1 = data;
	}
	else if (num == 2)
	{
		buf[i++] = 0x31;
		w25q.status_reg_2 = data;
	}
	else
	{
		buf[i++] = 0x11;
		w25q.status_reg_3 = data;
	}
	buf[i++] = data;
}

static bool waitForWriteEnd(void)
{
	buf[i++] = 0x05;
	do
	{
		w25q.status_reg_1 = buf[i++] = W25Q_DUMMY_BYTE;
	} while ((w25q.status_reg_1 & 0x01) == 0x01);
}
