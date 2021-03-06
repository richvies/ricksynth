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


#include "dma.h"
#include "clk.h"
#include "irq.h"


typedef struct
{
  bool init;
  DMA_HandleTypeDef hal;
  const dma_hw_info_t *hw;
  irq_priority_e irq_priority;
} dma_handle_t;


static dma_handle_t handles[DMA_NUM_OF_STREAM] = {0};

static const uint32_t dir_to_hal[DMA_DIR_NUM_OF] =
{
  DMA_PERIPH_TO_MEMORY,
  DMA_MEMORY_TO_PERIPH,
  DMA_MEMORY_TO_MEMORY
};
static const uint32_t size_to_hal[DMA_DATA_SIZE_NUM_OF] =
{
  DMA_PDATAALIGN_BYTE,
  DMA_PDATAALIGN_HALFWORD,
  DMA_PDATAALIGN_WORD,
};
static const uint32_t ch_to_dma_ch[DMA_NUM_OF_CH] =
{
  DMA_CHANNEL_0,
  DMA_CHANNEL_1,
  DMA_CHANNEL_2,
  DMA_CHANNEL_3,
  DMA_CHANNEL_4,
  DMA_CHANNEL_5,
  DMA_CHANNEL_6,
  DMA_CHANNEL_7,
};
static const uint32_t priority_to_dma_priority[PRIORITY_NUM_OF] =
{
  PRIORITY_VERY_HIGH,
  PRIORITY_HIGH,
  PRIORITY_MEDIUM,
  PRIORITY_LOW,
  PRIORITY_VERY_LOW,
};


static bool streamFromHal(DMA_HandleTypeDef *hdma, DMA_stream_e *ch);
static void configureHal(dma_handle_t *h, DMA_cfg_t *cfg);


bool DMA_init(DMA_stream_e stream, DMA_cfg_t *cfg)
{
  bool ret = false;
  dma_handle_t *h;

  if (stream >= DMA_NUM_OF_STREAM)
  {
    return false;
  }

  h = &handles[stream];

  if (true == h->init)
  {
    PRINTF_WARN("ch %u already init", stream);
    return true;
  }

  /* link to hw information */
  h->hw = &dma_hw_info[stream];

  /* configure peripheral */
  configureHal(h, cfg);

  /* enable clock and irq */
  CLK_periphEnable(h->hw->periph);

  if(HAL_OK == HAL_DMA_Init(&h->hal))
  {
    h->init = true;
    ret = true;
  }

  IRQ_config(h->hw->irq_num, h->irq_priority);
  IRQ_enable(h->hw->irq_num);

  return ret;
}

void* DMA_getHandle(DMA_stream_e stream)
{
  if (stream >= DMA_NUM_OF_STREAM)
  {
    return NULL;
  }

  return &(handles[stream].hal);
}

bool DMA_deinit(DMA_stream_e stream)
{
  bool ret = false;
  dma_handle_t *h;

  if (stream >= DMA_NUM_OF_STREAM)
  {
    return false;
  }

  h = &handles[stream];

  if (HAL_OK == HAL_DMA_DeInit(&h->hal))
  {
    h->init = false;
    ret = true;

    /* disable irq but leave clock running for other streams */
    IRQ_disable(h->hw->irq_num);
  }

  return ret;
}


static bool streamFromHal(DMA_HandleTypeDef *hdma, DMA_stream_e *ch)
{
  bool ret = false;
  DMA_stream_e i;

  for (i = DMA_STREAM_FIRST; i < DMA_NUM_OF_STREAM; i++)
  {
    if (hdma == &handles[i].hal)
    {
      *ch = i;
      ret = true;
      break;
    }
  }

  return ret;
}

static void configureHal(dma_handle_t *h, DMA_cfg_t *cfg)
{
  h->hal.Instance = h->hw->inst;
  h->hal.Parent   = cfg->parent_handle;
  h->irq_priority = cfg->priority;

  h->hal.Init.Channel = ch_to_dma_ch[cfg->channel];
  h->hal.Init.Direction = dir_to_hal[cfg->dir];
  h->hal.Init.PeriphInc = cfg->inc_periph_addr ? DMA_PINC_ENABLE : DMA_PINC_DISABLE;
  h->hal.Init.MemInc = cfg->inc_mem_addr ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
  h->hal.Init.PeriphDataAlignment = size_to_hal[cfg->periph_data_size];
  h->hal.Init.MemDataAlignment = size_to_hal[cfg->mem_data_size];
  h->hal.Init.Mode = cfg->circular_mode ? DMA_CIRCULAR : DMA_NORMAL;
  h->hal.Init.Priority = priority_to_dma_priority[cfg->priority];
  h->hal.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
}


void DMA1_Stream0_IRQHandler( void )
{
  HAL_DMA_IRQHandler(&handles[DMA_1_STREAM_0].hal);
}

void DMA1_Stream6_IRQHandler( void )
{
  HAL_DMA_IRQHandler(&handles[DMA_1_STREAM_6].hal);
}

void DMA2_Stream0_IRQHandler( void )
{
  HAL_DMA_IRQHandler(&handles[DMA_2_STREAM_0].hal);
}

void DMA2_Stream3_IRQHandler( void )
{
  HAL_DMA_IRQHandler(&handles[DMA_2_STREAM_3].hal);
}
