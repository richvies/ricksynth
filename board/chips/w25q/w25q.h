#ifndef _W25Q_H
#define _W25Q_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "spi.h"

	// printf("w25q Page Size: %d Bytes\r\n", w25q.page_size);
	// printf("w25q Page Count: %d\r\n", w25q.page_count);
	// printf("w25q Sector Size: %d Bytes\r\n", w25q.sector_size);
	// printf("w25q Sector Count: %d\r\n", w25q.sector_count);
	// printf("w25q Block Size: %d Bytes\r\n", w25q.block_size);
	// printf("w25q Block Count: %d\r\n", w25q.block_count);
	// printf("w25q Capacity: %d KiloBytes\r\n", w25q.size_in_kbytes);
	// printf("w25q Init Done\r\n");

typedef enum
{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
	NUM_OF_W25Q,
} W25Q_id_e;


typedef struct
{
	W25Q_id_e id;
	uint8_t 	uniqe_id[8];
	uint16_t 	page_size;
	uint32_t 	page_count;
	uint32_t 	sector_size;
	uint32_t 	sector_count;
	uint32_t 	block_size;
	uint32_t 	block_count;
	uint32_t 	size_in_kbytes;
	uint8_t 	status_reg_1;
	uint8_t 	status_reg_2;
	uint8_t 	status_reg_3;
} W25Q_t;


extern W25Q_t w25q;


bool W25Q_init(SPI_ch_e ch);

void W25Q_eraseChip(void);
void W25Q_eraseSector(uint32_t addr);
void W25Q_eraseBlock(uint32_t addr);

bool W25Q_isEmptyPage(uint32_t addr, uint32_t offset, uint32_t len);
bool W25Q_isEmptySector(uint32_t addr, uint32_t offset, uint32_t len);
bool W25Q_isEmptyBlock(uint32_t addr, uint32_t offset, uint32_t len);

void W25Q_writeByte(uint8_t data, uint32_t addr);
void W25Q_writePage(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);
void W25Q_writeSector(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);
void W25Q_writeBlock(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);

void W25Q_readBytes(uint8_t *data, uint32_t addr, uint32_t len);
void W25Q_readPage(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);
void W25Q_readSector(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);
void W25Q_readBlock(uint8_t *data, uint32_t addr, uint32_t offset, uint32_t len);

uint32_t W25Q_pageToSector(uint32_t addr);
uint32_t W25Q_pageToBlock(uint32_t addr);
uint32_t W25Q_sectorToBlock(uint32_t addr);
uint32_t W25Q_sectorToPage(uint32_t addr);
uint32_t W25Q_blockToPage(uint32_t addr);


#ifdef __cplusplus
}
#endif


#endif
