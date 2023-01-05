#ifndef _W25Q_H
#define _W25Q_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "spi.h"


#define W25Q_READ_SIZE			(1)
#define W25Q_PROG_SIZE			(256)
#define W25Q_SECTOR_SIZE		(4096)
#define W25Q_SECTOR_COUNT		(256*16)


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
															 void *data,
															 uint32_t len);
extern bool W25Q_readSector(uint32_t sector,
														uint32_t offset,
														void *data,
														uint32_t len);


#ifdef __cplusplus
}
#endif


#endif
