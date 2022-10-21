#include "w25q.h"


#define W25Q_DUMMY_BYTE 0xA5


#define TX(t, l) {xfer.dir = SPI_WRITE; xfer.tx_data = t; xfer.length = l; SPI_xferBlocking(spi_ch, &xfer);}
#define RX(r, l) {xfer.dir = SPI_READ; xfer.rx_data = r; xfer.length = l; SPI_xferBlocking(spi_ch, &xfer);}
#define TXRX(t, r, l) {xfer.dir = SPI_WRITE_READ; xfer.tx_data = t; xfer.rx_data = r; xfer.length = l; SPI_xferBlocking(spi_ch, &xfer);}


extern W25Q_t w25q;

static SPI_ch_e spi_ch;
static SPI_xfer_info_t xfer;


static uint32_t readID(void);
static void readuniqe_id(void);
static void writeEnable(void);
static void writeDisable(void);
static uint8_t readStatusRegister(uint8_t num);
static void writeStatusRegister(uint8_t num, uint8_t data);
static void waitForWriteEnd(void);


bool W25Q_init(SPI_ch_e ch, IO_num_e cs_pin)
{
	uint16_t id;
	bool ret = false;

	spi_ch = ch;
	xfer.cs_pin = cs_pin;

	id = readID();

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

	readuniqe_id();
	readStatusRegister(1);
	readStatusRegister(2);
	readStatusRegister(3);

	return true;
}


void W25Q_eraseChip(void)
{
	writeEnable();
	W25Q_spi(0xC7);
	waitForWriteEnd();
}

void W25Q_eraseSector(uint32_t addr)
{
	waitForWriteEnd();
	addr = addr * w25q.sector_size;
	writeEnable();
	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0x21);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0x20);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	waitForWriteEnd();
}

void W25Q_eraseBlock(uint32_t addr)
{
	waitForWriteEnd();
	addr = addr * w25q.sector_size * 16;
	writeEnable();
	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0xDC);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0xD8);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	waitForWriteEnd();
}


bool W25Q_isEmptyPage(uint32_t addr, uint32_t offset, uint32_t len)
{
	if (((len + offset) > w25q.page_size) || (len == 0))
		len = w25q.page_size - offset;
	uint8_t data[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = offset; i < w25q.page_size; i += sizeof(data))
	{
		WorkAddress = (i + addr * w25q.page_size);
		if (w25q.id >= W25Q256)
		{
			W25Q_spi(0x0C);
			W25Q_spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25Q_spi(0x0B);
		}
		W25Q_spi((WorkAddress & 0xFF0000) >> 16);
		W25Q_spi((WorkAddress & 0xFF00) >> 8);
		W25Q_spi(WorkAddress & 0xFF);
		W25Q_spi(0);
		HAL_SPI_Receive(&_W25Q_SPI, data, sizeof(data), 100);
		for (uint8_t x = 0; x < sizeof(data); x++)
		{
			if (data[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25q.page_size + offset) % sizeof(data) != 0)
	{
		i -= sizeof(data);
		for (; i < w25q.page_size; i++)
		{
			WorkAddress = (i + addr * w25q.page_size);
			W25Q_spi(0x0B);
			if (w25q.id >= W25Q256)
			{
				W25Q_spi(0x0C);
				W25Q_spi((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25Q_spi(0x0B);
			}
			W25Q_spi((WorkAddress & 0xFF0000) >> 16);
			W25Q_spi((WorkAddress & 0xFF00) >> 8);
			W25Q_spi(WorkAddress & 0xFF);
			W25Q_spi(0);
			HAL_SPI_Receive(&_W25Q_SPI, data, 1, 100);
			if (data[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	return true;
NOT_EMPTY:
	return false;
}

bool W25Q_isEmptySector(uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.sector_size) || (len == 0))
		len = w25q.sector_size;
	uint8_t data[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = offset; i < w25q.sector_size; i += sizeof(data))
	{
		WorkAddress = (i + addr * w25q.sector_size);
		if (w25q.id >= W25Q256)
		{
			W25Q_spi(0x0C);
			W25Q_spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25Q_spi(0x0B);
		}
		W25Q_spi((WorkAddress & 0xFF0000) >> 16);
		W25Q_spi((WorkAddress & 0xFF00) >> 8);
		W25Q_spi(WorkAddress & 0xFF);
		W25Q_spi(0);
		HAL_SPI_Receive(&_W25Q_SPI, data, sizeof(data), 100);
		for (uint8_t x = 0; x < sizeof(data); x++)
		{
			if (data[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25q.sector_size + offset) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < w25q.sector_size; i++)
		{
			WorkAddress = (i + addr * w25q.sector_size);
			if (w25q.id >= W25Q256)
			{
				W25Q_spi(0x0C);
				W25Q_spi((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25Q_spi(0x0B);
			}
			W25Q_spi((WorkAddress & 0xFF0000) >> 16);
			W25Q_spi((WorkAddress & 0xFF00) >> 8);
			W25Q_spi(WorkAddress & 0xFF);
			W25Q_spi(0);
			HAL_SPI_Receive(&_W25Q_SPI, pBuffer, 1, 100);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	return true;
NOT_EMPTY:
	return false;
}

bool W25Q_isEmptyBlock(uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.block_size) || (len == 0))
		len = w25q.block_size;
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = offset; i < w25q.block_size; i += sizeof(pBuffer))
	{
		WorkAddress = (i + addr * w25q.block_size);

		if (w25q.id >= W25Q256)
		{
			W25Q_spi(0x0C);
			W25Q_spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25Q_spi(0x0B);
		}
		W25Q_spi((WorkAddress & 0xFF0000) >> 16);
		W25Q_spi((WorkAddress & 0xFF00) >> 8);
		W25Q_spi(WorkAddress & 0xFF);
		W25Q_spi(0);
		HAL_SPI_Receive(&_W25Q_SPI, pBuffer, sizeof(pBuffer), 100);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25q.block_size + offset) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < w25q.block_size; i++)
		{
			WorkAddress = (i + addr * w25q.block_size);

			if (w25q.id >= W25Q256)
			{
				W25Q_spi(0x0C);
				W25Q_spi((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25Q_spi(0x0B);
			}
			W25Q_spi((WorkAddress & 0xFF0000) >> 16);
			W25Q_spi((WorkAddress & 0xFF00) >> 8);
			W25Q_spi(WorkAddress & 0xFF);
			W25Q_spi(0);
			HAL_SPI_Receive(&_W25Q_SPI, pBuffer, 1, 100);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	return true;
NOT_EMPTY:
	return false;
}


void W25Q_writeByte(uint8_t data, uint32_t addr)
{
	waitForWriteEnd();
	writeEnable();

	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0x12);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0x02);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	W25Q_spi(data);
	waitForWriteEnd();
}

void W25Q_writePage(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if (((len + offset) > w25q.page_size) || (len == 0))
		len = w25q.page_size - offset;
	if ((offset + len) > w25q.page_size)
		len = w25q.page_size - offset;
	waitForWriteEnd();
	writeEnable();
	addr = (addr * w25q.page_size) + offset;
	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0x12);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0x02);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	HAL_SPI_Transmit(&_W25Q_SPI, data, len, 100);
	waitForWriteEnd();
}

void W25Q_writeSector(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.sector_size) || (len == 0))
		len = w25q.sector_size;
	if (offset >= w25q.sector_size)
	{
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((offset + len) > w25q.sector_size)
		BytesToWrite = w25q.sector_size - offset;
	else
		BytesToWrite = len;
	StartPage = W25Q_sectorToPage(addr) + (offset / w25q.page_size);
	LocalOffset = offset % w25q.page_size;
	do
	{
		W25Q_writePage(data, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= w25q.page_size - LocalOffset;
		data += w25q.page_size - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
}

void W25Q_writeBlock(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.block_size) || (len == 0))
		len = w25q.block_size;
	if (offset >= w25q.block_size)
	{
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((offset + len) > w25q.block_size)
		BytesToWrite = w25q.block_size - offset;
	else
		BytesToWrite = len;
	StartPage = W25Q_blockToPage(addr) + (offset / w25q.page_size);
	LocalOffset = offset % w25q.page_size;
	do
	{
		W25Q_writePage(data, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= w25q.page_size - LocalOffset;
		data += w25q.page_size - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
}


void W25Q_readBytes(uint8_t *data, uint32_t addr, uint32_t len)
{
	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0x0C);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0x0B);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	W25Q_spi(0);
	HAL_SPI_Receive(&_W25Q_SPI, data, len, 2000);
}

void W25Q_readPage(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.page_size) || (len == 0))
		len = w25q.page_size;
	if ((offset + len) > w25q.page_size)
		len = w25q.page_size - offset;
	addr = addr * w25q.page_size + offset;
	if (w25q.id >= W25Q256)
	{
		W25Q_spi(0x0C);
		W25Q_spi((addr & 0xFF000000) >> 24);
	}
	else
	{
		W25Q_spi(0x0B);
	}
	W25Q_spi((addr & 0xFF0000) >> 16);
	W25Q_spi((addr & 0xFF00) >> 8);
	W25Q_spi(addr & 0xFF);
	W25Q_spi(0);
	HAL_SPI_Receive(&_W25Q_SPI, data, len, 100);
}

void W25Q_readSector(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.sector_size) || (len == 0))
		len = w25q.sector_size;
	if (offset >= w25q.sector_size)
	{
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((offset + len) > w25q.sector_size)
		BytesToRead = w25q.sector_size - offset;
	else
		BytesToRead = len;
	StartPage = W25Q_sectorToPage(addr) + (offset / w25q.page_size);
	LocalOffset = offset % w25q.page_size;
	do
	{
		W25Q_readPage(data, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= w25q.page_size - LocalOffset;
		data += w25q.page_size - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
}

void W25Q_readBlock(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len)
{
	if ((len > w25q.block_size) || (len == 0))
		len = w25q.block_size;
	if (offset >= w25q.block_size)
	{
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((offset + len) > w25q.block_size)
		BytesToRead = w25q.block_size - offset;
	else
		BytesToRead = len;
	StartPage = W25Q_blockToPage(addr) + (offset / w25q.page_size);
	LocalOffset = offset % w25q.page_size;
	do
	{
		W25Q_readPage(data, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= w25q.page_size - LocalOffset;
		data += w25q.page_size - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
}


uint32_t W25Q_pageToSector(uint32_t addr)
{
	return ((addr * w25q.page_size) / w25q.sector_size);
}

uint32_t W25Q_pageToBlock(uint32_t addr)
{
	return ((addr * w25q.page_size) / w25q.block_size);
}

uint32_t W25Q_sectorToBlock(uint32_t addr)
{
	return ((addr * w25q.sector_size) / w25q.block_size);
}

uint32_t W25Q_sectorToPage(uint32_t addr)
{
	return (addr * w25q.sector_size) / w25q.page_size;
}

uint32_t W25Q_blockToPage(uint32_t addr)
{
	return (addr * w25q.block_size) / w25q.page_size;
}


static uint32_t readID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	W25Q_spi(0x9F);
	Temp0 = W25Q_spi(W25Q_DUMMY_BYTE);
	Temp1 = W25Q_spi(W25Q_DUMMY_BYTE);
	Temp2 = W25Q_spi(W25Q_DUMMY_BYTE);
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return Temp;
}

static void readuniqe_id(void)
{
	W25Q_spi(0x4B);
	for (uint8_t i = 0; i < 4; i++)
		W25Q_spi(W25Q_DUMMY_BYTE);
	for (uint8_t i = 0; i < 8; i++)
		w25q.uniqe_id[i] = W25Q_spi(W25Q_DUMMY_BYTE);
}

static void writeEnable(void)
{
	W25Q_spi(0x06);
}

static void writeDisable(void)
{
	W25Q_spi(0x04);
}

static uint8_t readStatusRegister(uint8_t num)
{
	uint8_t status = 0;
	if (num == 1)
	{
		W25Q_spi(0x05);
		status = W25Q_spi(W25Q_DUMMY_BYTE);
		w25q.status_reg_1 = status;
	}
	else if (num == 2)
	{
		W25Q_spi(0x35);
		status = W25Q_spi(W25Q_DUMMY_BYTE);
		w25q.status_reg_2 = status;
	}
	else
	{
		W25Q_spi(0x15);
		status = W25Q_spi(W25Q_DUMMY_BYTE);
		w25q.status_reg_3 = status;
	}
	return status;
}

static void writeStatusRegister(uint8_t num, uint8_t data)
{
	if (num == 1)
	{
		W25Q_spi(0x01);
		w25q.status_reg_1 = data;
	}
	else if (num == 2)
	{
		W25Q_spi(0x31);
		w25q.status_reg_2 = data;
	}
	else
	{
		W25Q_spi(0x11);
		w25q.status_reg_3 = data;
	}
	W25Q_spi(data);
}

static void waitForWriteEnd(void)
{
	W25Q_spi(0x05);
	do
	{
		w25q.status_reg_1 = W25Q_spi(W25Q_DUMMY_BYTE);
	} while ((w25q.status_reg_1 & 0x01) == 0x01);
}
