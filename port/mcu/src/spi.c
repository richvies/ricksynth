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


#include "spi.h"

#include "common.h"

#include "mcu_private.h"

#include "clk.h"
#include "dma.h"
#include "io.h"
#include "irq.h"
#include "tim.h"


typedef enum
{
  WRITE,
  READ,
  WRITE_READ,
} spi_dir_e;


typedef struct
{
  bool init;
  SPI_HandleTypeDef hal;
  const spi_hw_info_t *hw;
  SPI_xfer_info_t *xfer;

  /* xfer queue */
  SPI_xfer_info_t *q[3];
  spi_dir_e q_dir[3];
  uint8_t q_n;
  uint8_t q_tail;
} spi_handle_t;


static spi_handle_t handles[SPI_NUM_OF_CH] = {0};

static const uint32_t master_mode_to_hal[SPI_NUM_OF_MASTER_MODES] =
{
  SPI_MODE_MASTER,
  SPI_MODE_SLAVE,
};
static const uint32_t clk_polarity_to_hal[SPI_NUM_OF_CLK_POLARITY] =
{
  SPI_POLARITY_LOW,
  SPI_POLARITY_HIGH,
};
static const uint32_t clk_phase_to_hal[SPI_NUM_OF_CLK_PHASES] =
{
  SPI_PHASE_1EDGE,
  SPI_PHASE_2EDGE,
};
static const uint32_t bit_order_to_hal[SPI_NUM_OF_BIT_ORDER] =
{
  SPI_FIRSTBIT_MSB,
  SPI_FIRSTBIT_LSB,
};
static const uint32_t prescaler_index_to_hal[8] =
{
  SPI_BAUDRATEPRESCALER_2,
  SPI_BAUDRATEPRESCALER_4,
  SPI_BAUDRATEPRESCALER_8,
  SPI_BAUDRATEPRESCALER_16,
  SPI_BAUDRATEPRESCALER_32,
  SPI_BAUDRATEPRESCALER_64,
  SPI_BAUDRATEPRESCALER_128,
  SPI_BAUDRATEPRESCALER_256,
};

static const uint8_t  PrescTable[sizeof(prescaler_index_to_hal)] =
{1, 2, 3, 4, 5, 6, 7, 8};


static bool writeIrq      (SPI_ch_e ch, uint8_t *d, uint16_t l);
static bool readIrq       (SPI_ch_e ch, uint8_t *d, uint16_t l);
static bool writeReadIrq  (SPI_ch_e ch, uint8_t *t, uint8_t *r, uint16_t l);
static bool writeDma      (SPI_ch_e ch, uint8_t *d, uint16_t l);
static bool readDma       (SPI_ch_e ch, uint8_t *d, uint16_t l);
static bool writeReadDma  (SPI_ch_e ch, uint8_t *t, uint8_t *r, uint16_t l);

static bool channelFromHal(SPI_HandleTypeDef *hspi, SPI_ch_e *ch);
static bool channelFromPeriph(periph_e periph, SPI_ch_e *ch);
static void configureHal(spi_handle_t *h, SPI_cfg_t *cfg);
static uint32_t hzToPrescaler(spi_handle_t *h, uint32_t hz);
static bool initDma(spi_handle_t *h);


bool SPI_init       (SPI_ch_e ch, SPI_cfg_t *cfg)
{
  bool ret = false;
  spi_handle_t *h;

  if (ch >= SPI_NUM_OF_CH)
  {
    return false;
  }

  h = &handles[ch];

  if (true == h->init)
  {
    PRINTF_WARN("ch %u already init", ch);
    return true;
  }

  /* link to hw information */
  h->hw = &spi_hw_info[ch];

  /* set config params */
  configureHal(h, cfg);

  if(HAL_OK == HAL_SPI_Init(&h->hal))
  {
    ret = true;
  }

  h->init = ret;

  return ret;
}

bool SPI_deInit     (SPI_ch_e ch)
{
  bool ret = false;
  spi_handle_t *h;

  if (ch >= SPI_NUM_OF_CH)
  {
    return false;
  }

  h = &handles[ch];

  if (HAL_OK == HAL_SPI_DeInit(&h->hal))
  {
    h->init = false;
    ret = true;

    if (h->hw->dma_rx_stream)
    {
      dma_deinit(h->hw->dma_rx_stream);
    }

    if (h->hw->dma_tx_stream)
    {
      dma_deinit(h->hw->dma_tx_stream);
    }
  }

  return ret;
}

bool SPI_isBusy     (SPI_ch_e ch)
{
  return (HAL_SPI_STATE_READY != HAL_SPI_GetState(&handles[ch].hal));
}

void SPI_task       (void)
{
  SPI_ch_e ch;
  spi_handle_t *h;
  bool ret;

  /* check if xfer on queue for each channel */
  for (ch = SPI_CH_FIRST; ch < SPI_NUM_OF_CH; ch++)
  {
    h = &handles[ch];

    if ((h->q_n)
    &&  (false == SPI_isBusy(ch)))
    {
      switch (h->q_dir[h->q_tail])
      {
      case WRITE:
        ret = SPI_write(ch, h->q[h->q_tail]);
        break;

      case READ:
        ret = SPI_read(ch, h->q[h->q_tail]);
        break;

      case WRITE_READ:
        ret = SPI_writeRead(ch, h->q[h->q_tail]);
        break;

      default:
        ret = false;
        break;
      }

      /* increment queue if xfer started */
      if (true == ret)
      {
        --h->q_n;
        ++h->q_tail;
        if (h->q_tail >= SIZEOF(h->q))
        {
          h->q_tail = 0;
        }
      }
    }
  }
}

bool SPI_write      (SPI_ch_e ch, SPI_xfer_info_t *info)
{
  bool ret = false;
  spi_handle_t *h = &handles[ch];

  /* if busy, add to queue */
  if (true == SPI_isBusy(ch))
  {
    if (h->q_n < SIZEOF(h->q))
    {
      h->q_dir[h->q_tail + h->q_n] = WRITE;
      h->q[h->q_tail + h->q_n++] = info;
      ret = true;
    }
  }
  /* otherwise send now */
  else
  {
    if (info->cs_pin)
    {
      IO_clear(info->cs_pin);
    }

    h->xfer = info;
    if (h->hw->dma_tx_stream)
    {
      ret = writeDma(ch, info->tx_data, info->length);
    }
    else
    {
      ret = writeIrq(ch, info->tx_data, info->length);
    }
  }

  return ret;
}

bool SPI_read       (SPI_ch_e ch, SPI_xfer_info_t *info)
{
  bool ret = false;
  spi_handle_t *h = &handles[ch];

  /* if busy, add to queue */
  if (true == SPI_isBusy(ch))
  {
    if (h->q_n < SIZEOF(h->q))
    {
      h->q_dir[h->q_tail + h->q_n] = READ;
      h->q[h->q_tail + h->q_n++] = info;
      ret = true;
    }
  }
  /* otherwise send now */
  else
  {
    if (info->cs_pin)
    {
      IO_clear(info->cs_pin);
    }

    h->xfer = info;
    if (h->hw->dma_rx_stream)
    {
      ret = readDma(ch, info->rx_data, info->length);
    }
    else
    {
      ret = readIrq(ch, info->rx_data, info->length);
    }
  }

  return ret;
}

bool SPI_writeRead  (SPI_ch_e ch, SPI_xfer_info_t *info)
{
  bool ret = false;
  spi_handle_t *h = &handles[ch];

  /* if busy, add to queue */
  if (true == SPI_isBusy(ch))
  {
    if (h->q_n < SIZEOF(h->q))
    {
      h->q_dir[h->q_tail + h->q_n] = WRITE_READ;
      h->q[h->q_tail + h->q_n++] = info;
      ret = true;
    }
  }
  /* otherwise send now */
  else
  {
    if (info->cs_pin)
    {
      IO_clear(info->cs_pin);
    }

    handles[ch].xfer = info;
    if (handles[ch].hw->dma_tx_stream && handles[ch].hw->dma_rx_stream)
    {
      ret = writeReadDma(ch, info->tx_data, info->rx_data, info->length);
    }
    else
    {
      ret = writeReadIrq(ch, info->tx_data, info->rx_data, info->length);
    }
  }

  return ret;
}


static bool writeIrq      (SPI_ch_e ch, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_SPI_Transmit(&handles[ch].hal, d, l, 1000));
}

static bool readIrq       (SPI_ch_e ch, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_SPI_Receive(&handles[ch].hal, d, l, 1000));
}

static bool writeReadIrq  (SPI_ch_e ch, uint8_t *t, uint8_t *r, uint16_t l)
{
  return (HAL_OK == HAL_SPI_TransmitReceive(&handles[ch].hal, t, r, l, 1000));
}

static bool writeDma      (SPI_ch_e ch, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_SPI_Transmit_DMA(&handles[ch].hal, d, l));
}

static bool readDma       (SPI_ch_e ch, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_SPI_Receive_DMA(&handles[ch].hal, d, l));
}

static bool writeReadDma  (SPI_ch_e ch, uint8_t *t, uint8_t *r, uint16_t l)
{
  return (HAL_OK == HAL_SPI_TransmitReceive_DMA(&handles[ch].hal, t, r, l));
}


static bool channelFromHal(SPI_HandleTypeDef *hspi, SPI_ch_e *ch)
{
  bool ret = false;
  SPI_ch_e i;

  for (i = SPI_CH_FIRST; i < SPI_NUM_OF_CH; i++)
  {
    if (hspi == &handles[i].hal)
    {
      *ch = i;
      ret = true;
      break;
    }
  }

  return ret;
}

static bool channelFromPeriph(periph_e periph, SPI_ch_e *ch)
{
  bool ret = false;
  SPI_ch_e i;

  for (i = SPI_CH_FIRST; i < SPI_NUM_OF_CH; i++)
  {
    if (periph == handles[i].hw->periph)
    {
      *ch = i;
      ret = true;
      break;
    }
  }

  return ret;
}

static void configureHal(spi_handle_t *h, SPI_cfg_t *cfg)
{
  h->hal.Instance               = h->hw->inst;
  h->hal.Init.Mode              = master_mode_to_hal[cfg->master_mode];
  h->hal.Init.Direction         = SPI_DIRECTION_2LINES;
  h->hal.Init.DataSize          = SPI_DATASIZE_8BIT;
  h->hal.Init.CLKPolarity       = clk_polarity_to_hal[cfg->clk_polarity];
  h->hal.Init.CLKPhase          = clk_phase_to_hal[cfg->clk_phase];
  h->hal.Init.NSS               = SPI_NSS_SOFT;
  h->hal.Init.BaudRatePrescaler = hzToPrescaler(h, cfg->clk_speed_hz);
  h->hal.Init.FirstBit          = bit_order_to_hal[cfg->bit_order];
  h->hal.Init.TIMode            = SPI_TIMODE_DISABLED;
  h->hal.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  h->hal.Init.CRCPolynomial     = 1;
}

static uint32_t hzToPrescaler(spi_handle_t *h, uint32_t hz)
{
  uint8_t i = 0;
  uint32_t clk_hz = clk_getPeriphBaseClkHz(h->hw->periph);

  while ((clk_hz >> PrescTable[i] > hz)
  &&     (i < sizeof(PrescTable)))
  {
    ++i;
  }

  return prescaler_index_to_hal[i];
}

static bool initDma(spi_handle_t *h)
{
  bool ret = true;
  dma_cfg_t dma_cfg;

  dma_cfg.priority          = h->hw->irq_priority;
  dma_cfg.parent_handle     = &h->hal;
  dma_cfg.periph_data_size  = DMA_DATA_SIZE_8BIT;
  dma_cfg.mem_data_size     = DMA_DATA_SIZE_8BIT;
  dma_cfg.inc_mem_addr      = true;
  dma_cfg.inc_periph_addr   = false;
  dma_cfg.circular_mode     = false;

  if (h->hw->dma_rx_stream)
  {
    dma_cfg.dir = DMA_DIR_PERIPH_TO_MEM;
    dma_cfg.channel = h->hw->dma_rx_ch;
    ret &= dma_init(h->hw->dma_rx_stream, &dma_cfg);
    h->hal.hdmarx = dma_getHandle(h->hw->dma_rx_stream);
  }

  if (h->hw->dma_tx_stream)
  {
    dma_cfg.dir = DMA_DIR_MEM_TO_PERIPH;
    dma_cfg.channel = h->hw->dma_tx_ch;
    ret &= dma_init(h->hw->dma_tx_stream, &dma_cfg);
    h->hal.hdmatx = dma_getHandle(h->hw->dma_tx_stream);
  }

  return ret;
}


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  SPI_ch_e ch;
  spi_handle_t *h;
  IO_cfg_t io_cfg;

  if (false == channelFromHal(hspi, &ch))
  {
    return;
  }

  h  = &handles[ch];

  io_cfg.dir     = IO_DIR_OUT_PP;
  io_cfg.mode    = IO_MODE_PERIPH;
  io_cfg.pullup  = IO_PULL_NONE;
  io_cfg.speed   = IO_SPEED_FAST;
  io_cfg.extend  = &h->hw->io_cfg_ext;
  IO_configure(h->hw->mosi_pin, &io_cfg);
  IO_configure(h->hw->sck_pin, &io_cfg);

  io_cfg.dir     = IO_DIR_IN;
  IO_configure(h->hw->miso_pin, &io_cfg);

  clk_periphEnable(h->hw->periph);

  initDma(&handles[ch]);

  irq_config(h->hw->irq_num, h->hw->irq_priority);
  irq_enable(h->hw->irq_num);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  SPI_ch_e ch;
  spi_handle_t *h;

  if (false == channelFromHal(hspi, &ch))
  {
    return;
  }

  h  = &handles[ch];

  irq_disable(h->hw->irq_num);

  IO_deinit(h->hw->sck_pin);
  IO_deinit(h->hw->miso_pin);
  IO_deinit(h->hw->mosi_pin);

  clk_periphReset(h->hw->periph);
}


static void irqHandler(periph_e periph)
{
  SPI_ch_e ch;

  if (true == channelFromPeriph(periph, &ch))
  {
    HAL_SPI_IRQHandler(&handles[ch].hal);
  }
}

void SPI1_IRQHandler(void)
{
  irqHandler(periph_SPI_1);
}

void SPI2_IRQHandler(void)
{
  irqHandler(periph_SPI_2);
}

void SPI3_IRQHandler(void)
{
  irqHandler(periph_SPI_3);
}

void SPI4_IRQHandler(void)
{
  irqHandler(periph_SPI_4);
}


static void callXferCb(SPI_HandleTypeDef *hspi, bool error, bool done)
{
  SPI_ch_e ch;

  if (true == channelFromHal(hspi, &ch))
  {
    if (handles[ch].xfer->cb)
    {
      handles[ch].xfer->cb(error, done, handles[ch].xfer->ctx);
    }

    if (done)
    {
      if (handles[ch].xfer->cs_pin)
      {
        IO_set(handles[ch].xfer->cs_pin);
      }
    }
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, true);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, true);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, true);
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, false);
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, false);
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, false, false);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, true, true);
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
  callXferCb(hspi, true, true);
}
