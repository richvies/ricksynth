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


#include "i2c.h"

#include "mcu.h"
#include "clk.h"
#include "dma.h"
#include "io.h"
#include "irq.h"


typedef struct
{
  bool init;
  I2C_HandleTypeDef hal;
  const i2c_hw_info_t *hw;
  I2C_xfer_cb cb;
} i2c_handle_t;


static i2c_handle_t handles[I2C_NUM_OF_CH] = {0};

static const uint32_t address_mode_to_hal[I2C_NUM_OF_ADDRESS_MODES] =
{
  I2C_ADDRESSINGMODE_7BIT,
  I2C_ADDRESSINGMODE_10BIT,
};
static const uint32_t stretch_mode_to_hal[I2C_NUM_OF_STRETCH_MODES] =
{
  I2C_NOSTRETCH_ENABLE,
  I2C_NOSTRETCH_DISABLE,
};


static bool writeIrq   (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l);
static bool readIrq    (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l);
static bool writeDma   (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l);
static bool readDma    (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l);

static bool channelFromHal(I2C_HandleTypeDef *hi2c, I2C_ch_e *ch);
static bool channelFromPeriph(periph_e periph, I2C_ch_e *ch);
static void configureHal(i2c_handle_t *h, I2C_cfg_t *cfg);
static bool initDma(i2c_handle_t *h);


bool I2C_init   (I2C_ch_e ch, I2C_cfg_t *cfg)
{
  bool ret = false;
  i2c_handle_t *h;

  if (ch >= I2C_NUM_OF_CH)
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
  h->hw = &i2c_hw_info[ch];

  /* set config params */
  configureHal(h, cfg);

  if(HAL_OK == HAL_I2C_Init(&h->hal))
  {
    ret = initDma(h);
  }

  h->init = ret;

  return ret;
}

bool I2C_deInit (I2C_ch_e ch)
{
  bool ret = false;
  i2c_handle_t *h;

  if (ch >= I2C_NUM_OF_CH)
  {
    return false;
  }

  h = &handles[ch];

  if (HAL_OK == HAL_I2C_DeInit(&h->hal))
  {
    h->init = false;
    ret = true;

    if (h->hw->dma_rx_stream)
    {
      DMA_deinit(h->hw->dma_rx_stream);
    }

    if (h->hw->dma_tx_stream)
    {
      DMA_deinit(h->hw->dma_tx_stream);
    }
  }

  return ret;
}

bool I2C_isBusy (I2C_ch_e ch)
{
  return (HAL_I2C_STATE_READY == HAL_I2C_GetState(&handles[ch].hal));
}

bool I2C_write    (I2C_ch_e ch, I2C_xfer_info_t *info)
{
  bool ret = I2C_isBusy(ch);

  if (false == ret)
  {
    handles[ch].cb = info->cb;

    if (handles[ch].hw->dma_tx_stream)
    {
      ret = writeDma(ch, info->addr, info->data, info->length);
    }
    else
    {
      ret = writeIrq(ch, info->addr, info->data, info->length);
    }
  }

  return ret;
}

bool I2C_read     (I2C_ch_e ch, I2C_xfer_info_t *info)
{
  bool ret = I2C_isBusy(ch);

  if (false == ret)
  {
    handles[ch].cb = info->cb;

    if (handles[ch].hw->dma_rx_stream)
    {
      ret = writeDma(ch, info->addr, info->data, info->length);
    }
    else
    {
      ret = writeIrq(ch, info->addr, info->data, info->length);
    }
  }

  return ret;
}


static bool writeIrq   (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_IT(&handles[ch].hal, addr, d, l));
}

static bool readIrq    (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_I2C_Master_Receive_IT(&handles[ch].hal, addr, d, l));
}

static bool writeDma   (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_DMA(&handles[ch].hal, addr, d, l));
}

static bool readDma    (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  return (HAL_OK == HAL_I2C_Master_Receive_DMA(&handles[ch].hal, addr, d, l));
}


static bool channelFromHal(I2C_HandleTypeDef *hi2c, I2C_ch_e *ch)
{
  bool ret = false;
  I2C_ch_e i;

  for (i = I2C_CH_FIRST; i < I2C_NUM_OF_CH; i++)
  {
    if (hi2c == &handles[i].hal)
    {
      *ch = i;
      ret = true;
      break;
    }
  }

  return ret;
}

static bool channelFromPeriph(periph_e periph, I2C_ch_e *ch)
{
  bool ret = false;
  I2C_ch_e i;

  for (i = I2C_CH_FIRST; i < I2C_NUM_OF_CH; i++)
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

static void configureHal(i2c_handle_t *h, I2C_cfg_t *cfg)
{
  h->hal.Instance              = h->hw->inst;
  h->hal.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  h->hal.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  h->hal.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  h->hal.Init.ClockSpeed      = cfg->clk_speed_hz;
  h->hal.Init.OwnAddress1     = (uint32_t)cfg->own_address&0x0000FFFF;
  h->hal.Init.OwnAddress2     = 0;
  h->hal.Init.AddressingMode  = address_mode_to_hal[cfg->address_mode];
  h->hal.Init.NoStretchMode   = stretch_mode_to_hal[cfg->stretch_mode];
}

static bool initDma(i2c_handle_t *h)
{
  bool ret = true;
  DMA_cfg_t dma_cfg;

  dma_cfg.channel           = h->hw->dma_rx_ch;
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
    ret &= DMA_init(h->hw->dma_rx_stream, &dma_cfg);
  }

  if (h->hw->dma_tx_stream)
  {
    dma_cfg.dir = DMA_DIR_MEM_TO_PERIPH;
    ret &= DMA_init(h->hw->dma_tx_stream, &dma_cfg);
  }

  return ret;
}


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  I2C_ch_e ch;
  i2c_handle_t *h;
  IO_cfg_t io_cfg;

  if (false == channelFromHal(hi2c, &ch))
  {
    return;
  }

  h  = &handles[ch];

  io_cfg.dir     = IO_DIR_OUT_OD;
  io_cfg.mode    = IO_MODE_PERIPH;
  io_cfg.pullup  = IO_PULL_UP;
  io_cfg.speed   = IO_SPEED_FAST;
  io_cfg.extend  = &h->hw->io_cfg_ext;
  IO_configure(h->hw->sda_pin, &io_cfg);
  IO_configure(h->hw->scl_pin, &io_cfg);

  CLK_periphEnable(h->hw->periph);

  IRQ_config(h->hw->event_irq_num, h->hw->irq_priority);
  IRQ_config(h->hw->error_irq_num, h->hw->irq_priority);
  IRQ_enable(h->hw->event_irq_num);
  IRQ_enable(h->hw->error_irq_num);
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  I2C_ch_e ch;
  i2c_handle_t *h;

  if (false == channelFromHal(hi2c, &ch))
  {
    return;
  }

  h  = &handles[ch];

  IRQ_disable(h->hw->event_irq_num);
  IRQ_disable(h->hw->error_irq_num);

  IO_deinit(h->hw->sda_pin);
  IO_deinit(h->hw->scl_pin);

  CLK_periphReset(h->hw->periph);
}


static void irqHandler(periph_e periph, bool error)
{
  I2C_ch_e ch;

  if (true == channelFromPeriph(periph, &ch))
  {
    if (error)
    {
      HAL_I2C_ER_IRQHandler(&handles[ch].hal);
    }
    else
    {
      HAL_I2C_EV_IRQHandler(&handles[ch].hal);
    }
  }
}

void I2C1_EV_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_1, false);
}

void I2C1_ER_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_1, true);
}

void I2C2_EV_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_2, false);
}

void I2C2_ER_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_2, true);
}

void I2C3_EV_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_3, false);
}

void I2C3_ER_IRQHandler(void)
{
  irqHandler(PERIPH_I2C_3, true);
}


static void callXferCb(I2C_HandleTypeDef *hi2c, bool error)
{
  I2C_ch_e ch;

  if (true == channelFromHal(hi2c, &ch))
  {
    if (handles[ch].cb)
    {
      handles[ch].cb(error);
    }
  }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  callXferCb(hi2c, false);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  callXferCb(hi2c, false);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  callXferCb(hi2c, true);
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
  callXferCb(hi2c, true);
}
