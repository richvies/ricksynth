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


#include "adc.h"

#include "_hw_info.h"

#include "clk.h"
#include "dma.h"
#include "io.h"
#include "irq.h"


#define ADC_MAX_VAL   (1 << 12)


typedef struct
{
  bool init;
  ADC_HandleTypeDef hal;
  const adc_hw_info_t *hw;
} handle_t;

handle_t handles[ADC_NUM_OF_PERIPH] = {0};
static uint16_t vals[ADC_NUM_OF_CH] = {0};

static bool initDma(handle_t *h);


bool ADC_init   (void)
{
  ADC_ch_e ch;
  bool ret = false;
  uint8_t rank = 1;
  ADC_ChannelConfTypeDef cfg;
  handle_t *h = &handles[0];

  h->hw                               = &adc_hw_info[0];
  h->hal.Instance                     = h->hw->inst;
  h->hal.Init.ClockPrescaler          = ADC_CLOCK_SYNC_PCLK_DIV8;
  h->hal.Init.ContinuousConvMode      = ENABLE;
  h->hal.Init.DataAlign               = ADC_DATAALIGN_RIGHT;
  h->hal.Init.DiscontinuousConvMode   = DISABLE;
  h->hal.Init.DMAContinuousRequests   = ENABLE;
  h->hal.Init.EOCSelection            = ADC_EOC_SEQ_CONV;
  h->hal.Init.ExternalTrigConv        = ADC_EXTERNALTRIGCONV_T1_CC1;
  h->hal.Init.ExternalTrigConvEdge    = ADC_EXTERNALTRIGCONVEDGE_NONE;
  h->hal.Init.NbrOfConversion         = ADC_NUM_OF_CH;
  h->hal.Init.NbrOfDiscConversion     = 1;
  h->hal.Init.Resolution              = ADC_RESOLUTION_12B;
  h->hal.Init.ScanConvMode            = ENABLE;

  if(HAL_OK == HAL_ADC_Init(&h->hal))
  {
    for (ch = ADC_CH_FIRST; ch < ADC_NUM_OF_CH; ch++)
    {
      cfg.Channel       = adc_ch_info[ch].channel;
      cfg.Offset        = 0;
      cfg.Rank          = rank++;
      cfg.SamplingTime  = ADC_SAMPLETIME_480CYCLES;
      HAL_ADC_ConfigChannel(&h->hal, &cfg);
    }

    ret = true;
  }

  h->init = ret;

  return ret;
}

bool ADC_deInit (void)
{
  bool ret = false;
  handle_t *h = &handles[0];

  if (HAL_OK == HAL_ADC_DeInit(&h->hal))
  {
    h->init = false;
    ret = true;

    if (h->hw->dma_stream)
    {
      dma_deinit(h->hw->dma_stream);
    }
  }

  return ret;
}

bool ADC_start  (void)
{
  bool ret;
  handle_t *h = &handles[0];

  if (h->hw->dma_stream)
  {
    ret = HAL_ADC_Start_DMA(&h->hal, (uint32_t*)vals, ADC_NUM_OF_CH);
  }
  else
  {
    ret = HAL_ADC_Start_IT(&h->hal);
  }

  return ret;
}

bool ADC_stop   (void)
{
  bool ret;
  handle_t *h = &handles[0];

  if (h->hw->dma_stream)
  {
    ret = HAL_ADC_Stop_DMA(&h->hal);
  }
  else
  {
    ret = HAL_ADC_Stop_IT(&h->hal);
  }

  return ret;
}


static bool initDma(handle_t *h)
{
  bool ret = true;
  dma_cfg_t dma_cfg;

  dma_cfg.priority          = h->hw->irq_priority;
  dma_cfg.parent_handle     = &h->hal;
  dma_cfg.periph_data_size  = DMA_DATA_SIZE_16BIT;
  dma_cfg.mem_data_size     = DMA_DATA_SIZE_16BIT;
  dma_cfg.inc_mem_addr      = true;
  dma_cfg.inc_periph_addr   = false;
  dma_cfg.circular_mode     = true;

  if (h->hw->dma_stream)
  {
    dma_cfg.dir = DMA_DIR_PERIPH_TO_MEM;
    dma_cfg.channel = h->hw->dma_ch;
    ret &= dma_init(h->hw->dma_stream, &dma_cfg);
    h->hal.DMA_Handle = dma_getHandle(h->hw->dma_stream);
  }

  return ret;
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  ADC_ch_e ch;
  IO_cfg_t io_cfg;
  handle_t *h = &handles[0];

  for (ch = ADC_CH_FIRST; ch < ADC_NUM_OF_CH; ch++)
  {
    io_cfg.dir     = IO_DIR_IN;
    io_cfg.mode    = IO_MODE_ANALOG;
    io_cfg.pullup  = IO_PULL_NONE;
    io_cfg.speed   = IO_SPEED_FAST;
    io_cfg.extend  = NULL;
    IO_configure(adc_ch_info[ch].io_pin, &io_cfg);
  }

  clk_periphEnable(h->hw->periph);

  initDma(h);

  irq_config(h->hw->irq_num, h->hw->irq_priority);
  irq_enable(h->hw->irq_num);
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  ADC_ch_e ch;
  handle_t *h = &handles[0];

  irq_disable(h->hw->irq_num);

  for (ch = ADC_CH_FIRST; ch < ADC_NUM_OF_CH; ch++)
  {
    IO_deinit(adc_ch_info[ch].io_pin);
  }

  clk_periphReset(h->hw->periph);
}
