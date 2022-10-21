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


#ifndef __DMA_H
#define __DMA_H


#include "mcu_private.h"


typedef enum
{
  DMA_DIR_PERIPH_TO_MEM,
  DMA_DIR_MEM_TO_PERIPH,
  DMA_DIR_MEM_TO_MEM,
  DMA_DIR_NUM_OF,
} dma_dir_e;

typedef enum
{
  DMA_DATA_SIZE_8BIT,
  DMA_DATA_SIZE_16BIT,
  DMA_DATA_SIZE_32BIT,
  DMA_DATA_SIZE_NUM_OF,
} dma_data_size_e;


typedef struct
{
  void *parent_handle;
  irq_priority_e priority;

  dma_ch_e        channel;
  dma_dir_e       dir;
  bool            inc_periph_addr;
  bool            inc_mem_addr;
  dma_data_size_e periph_data_size;
  dma_data_size_e mem_data_size;
  bool            circular_mode;
} dma_cfg_t;


bool    dma_init(dma_stream_e stream, dma_cfg_t *cfg);
void*   dma_getHandle(dma_stream_e stream);
bool    dma_deinit(dma_stream_e stream);


#endif
