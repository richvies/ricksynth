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
	NUM_OF_W25Q,
} W25Q_id_e;


extern bool W25Q_init(void);

extern bool W25Q_eraseSector(uint32_t sector);
extern bool W25Q_programSector(uint32_t sector,
															 uint32_t offset,
															 uint8_t *data,
															 uint32_t len);
extern bool W25Q_readSector(uint32_t sector,
														uint32_t offset,
														uint8_t *data,
														uint32_t len);


#ifdef __cplusplus
}
#endif


#endif
