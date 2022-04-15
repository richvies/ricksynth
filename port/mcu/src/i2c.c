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
#include "io.h"
#include "irq.h"


typedef struct
{
  bool init;
  I2C_HandleTypeDef hal;
  const i2c_hw_info_t *hw;
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


static bool channelFromHal(I2C_HandleTypeDef *hi2c, I2C_ch_e *ch);
static void configureHal(i2c_handle_t *h, I2C_config_t *cfg);


bool I2C_init   (I2C_ch_e ch, I2C_config_t *cfg)
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

  /* configure peripheral */
  configureHal(h, cfg);

  if(HAL_OK == HAL_I2C_Init(&h->hal))
  {
    h->init = true;
    ret = true;
  }

  return ret;
}

bool I2C_deInit (I2C_ch_e ch)
{
  bool ret = false;

  if (HAL_OK == HAL_I2C_DeInit(&handles[ch].hal))
  {
    handles[ch].init = false;
    ret = true;
  }

  return ret;
}

bool I2C_isBusy (I2C_ch_e ch)
{
  bool ret = false;

  if (HAL_I2C_STATE_READY == HAL_I2C_GetState(&handles[ch].hal))
  {
    ret = true;
  }

  return ret;
}


bool I2C_write    (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Transmit(&handles[ch].hal, addr, d, l, 1000)))
  {
    ret = true;
  }

  return ret;
}

bool I2C_read     (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Receive(&handles[ch].hal, addr, d, l, 1000)))
  {
    ret = true;
  }

  return ret;
}

bool I2C_writeIrq (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Transmit_IT(&handles[ch].hal, addr, d, l)))
  {
    ret = true;
  }

  return ret;
}

bool I2C_readIrq  (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Receive_IT(&handles[ch].hal, addr, d, l)))
  {
    ret = true;
  }

  return ret;
}

bool I2C_writeDma (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Transmit_DMA(&handles[ch].hal, addr, d, l)))
  {
    ret = true;
  }

  return ret;
}

bool I2C_readDma  (I2C_ch_e ch, uint16_t addr, uint8_t *d, uint16_t l)
{
  bool ret = false;

  if ((false == I2C_isBusy(ch))
  &&  (HAL_OK == HAL_I2C_Master_Receive_DMA(&handles[ch].hal, addr, d, l)))
  {
    ret = true;
  }

  return ret;
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

static void configureHal(i2c_handle_t *h, I2C_config_t *cfg)
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


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  I2C_ch_e ch;
  i2c_handle_t *h;
  IO_cfg_t cfg;

  if (false == channelFromHal(hi2c, &ch))
  {
    return;
  }

  h  = &handles[ch];

  cfg.dir     = IO_DIR_OUT_OD;
  cfg.mode    = IO_MODE_PERIPH;
  cfg.pullup  = IO_PULL_UP;
  cfg.speed   = IO_SPEED_FAST;
  cfg.extend  = &h->hw->io_cfg_ext;
  IO_configure(h->hw->sda_pin, &cfg);
  IO_configure(h->hw->scl_pin, &cfg);

  CLK_periphEnable(h->hw->periph);

  IRQ_config(h->hw->event_irq_num, h->hw->event_irq_priority);
  IRQ_config(h->hw->error_irq_num, h->hw->error_irq_priority);
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
}

void I2C3_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&handles[0].hal);
}

void I2C3_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&handles[0].hal);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  UNUSED(I2cHandle);

  //TODO
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  UNUSED(I2cHandle);

  //TODO
}
