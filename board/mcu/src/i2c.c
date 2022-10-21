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
} i2c_dir_e;


typedef struct
{
  bool init;
  I2C_xfer_info_t *xfer;
  I2C_HandleTypeDef hal;
  const i2c_hw_info_t *hw;

  /* xfer queue */
  I2C_xfer_info_t *q[3];
  i2c_dir_e q_dir[3];
  uint8_t q_n;
  uint8_t q_tail;
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


static bool writeIrq   (i2c_handle_t *h, I2C_xfer_info_t *info);
static bool readIrq    (i2c_handle_t *h, I2C_xfer_info_t *info);
static bool writeDma   (i2c_handle_t *h, I2C_xfer_info_t *info);
static bool readDma    (i2c_handle_t *h, I2C_xfer_info_t *info);

static bool channelFromHal(I2C_HandleTypeDef *hi2c, I2C_ch_e *ch);
static void configureHal(i2c_handle_t *h, I2C_cfg_t *cfg);
static bool initDma(i2c_handle_t *h);


static void HAL_I2C_ClearBusyFlagErrata_2_14_7(i2c_handle_t *h);


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
    /* point irq to this handle (h) for use in interrupt handler */
    irq_set_context(h->hw->event_irq_num, h);
    irq_set_context(h->hw->error_irq_num, h);

    ret = true;
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

  if (false == h->init)
  {
    PRINTF_WARN("ch %u already stopped", ch);
    return true;
  }

  if (HAL_OK == HAL_I2C_DeInit(&h->hal))
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

void I2C_task   (void)
{
  I2C_ch_e ch;
  i2c_handle_t *h;
  bool ret;

  /* check if xfer on queue for each channel */
  for (ch = I2C_CH_FIRST; ch < I2C_NUM_OF_CH; ch++)
  {
    h = &handles[ch];

    if ((h->q_n)
    &&  (false == I2C_isBusy(ch)))
    {
      switch (h->q_dir[h->q_tail])
      {
      case WRITE:
        ret = I2C_write(ch, h->q[h->q_tail]);
        break;

      case READ:
        ret = I2C_read(ch, h->q[h->q_tail]);
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
      else
      {
        PRINTF_WARN("task start xfer");
      }
    }
  }
}

bool I2C_isBusy (I2C_ch_e ch)
{
  return (HAL_I2C_STATE_READY != HAL_I2C_GetState(&handles[ch].hal));
}

bool I2C_write    (I2C_ch_e ch, I2C_xfer_info_t *info)
{
  bool ret = false;
  i2c_handle_t *h = &handles[ch];

  /* if busy, add to queue */
  if (true == I2C_isBusy(ch))
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
    h->xfer = info;
    if (h->hw->dma_tx_stream)
    {
      ret = writeDma(h, info);
    }
    else
    {
      ret = writeIrq(h, info);
    }
  }

  if (false == ret)
  {
    PRINTF_WARN("write");
  }

  return ret;
}

bool I2C_read     (I2C_ch_e ch, I2C_xfer_info_t *info)
{
  bool ret = false;
  i2c_handle_t *h = &handles[ch];

  /* if busy, add to queue */
  if (true == I2C_isBusy(ch))
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
    h->xfer = info;
    if (h->hw->dma_rx_stream)
    {
      ret = readDma(h, info);
    }
    else
    {
      ret = readIrq(h, info);
    }
  }

  if (false == ret)
  {
    PRINTF_WARN("read");
  }

  return ret;
}


static bool writeIrq   (i2c_handle_t *h, I2C_xfer_info_t *info)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_IT(
    &h->hal, info->addr, info->data, info->length));
}

static bool readIrq    (i2c_handle_t *h, I2C_xfer_info_t *info)
{
  return (HAL_OK == HAL_I2C_Master_Receive_IT(
    &h->hal, info->addr, info->data, info->length));
}

static bool writeDma   (i2c_handle_t *h, I2C_xfer_info_t *info)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_DMA(
    &h->hal, info->addr, info->data, info->length));
}

static bool readDma    (i2c_handle_t *h, I2C_xfer_info_t *info)
{
  return (HAL_OK == HAL_I2C_Master_Receive_DMA(
    &h->hal, info->addr, info->data, info->length));
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

static void configureHal(i2c_handle_t *h, I2C_cfg_t *cfg)
{
  h->hal.Instance             = h->hw->inst;
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

  clk_periphEnable(h->hw->periph);
  HAL_I2C_ClearBusyFlagErrata_2_14_7(h);

  initDma(&handles[ch]);

  irq_config(h->hw->event_irq_num, h->hw->irq_priority);
  irq_config(h->hw->error_irq_num, h->hw->irq_priority);
  irq_enable(h->hw->event_irq_num);
  irq_enable(h->hw->error_irq_num);
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

  irq_disable(h->hw->event_irq_num);
  irq_disable(h->hw->error_irq_num);

  IO_deinit(h->hw->sda_pin);
  IO_deinit(h->hw->scl_pin);

  clk_periphReset(h->hw->periph);
}


void i2c_event_irq_handler(void)
{
  i2c_handle_t *h = (i2c_handle_t*)irq_get_context(irq_get_current());

  if (h)
  {
    HAL_I2C_EV_IRQHandler(&h->hal);
  }
}

void i2c_error_irq_handler(void)
{
  i2c_handle_t *h = (i2c_handle_t*)irq_get_context(irq_get_current());

  if (h)
  {
    HAL_I2C_ER_IRQHandler(&h->hal);
  }
}

static void irq_end_call_callback(bool error)
{
  i2c_handle_t *h = (i2c_handle_t*)irq_get_context(irq_get_current());

  if (h)
  {
    if (h->xfer->cb)
    {
      h->xfer->cb(error, h->xfer->ctx);
    }
  }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  irq_end_call_callback(false);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  irq_end_call_callback(false);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  irq_end_call_callback(true);
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
  irq_end_call_callback(true);
}



/* Errata */
static void HAL_GPIO_WRITE_ODR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->ODR |= GPIO_Pin;
}

static void HAL_I2C_ClearBusyFlagErrata_2_14_7(i2c_handle_t *h)
{
  I2C_HandleTypeDef *hi2c = &h->hal;

  uint32_t SDA_PIN = IO_TO_PIN(h->hw->sda_pin);
  uint32_t SCL_PIN = IO_TO_PIN(h->hw->scl_pin);
  GPIO_InitTypeDef GPIO_InitStruct;

  // 1
  __HAL_I2C_DISABLE(hi2c);

  // 2
  GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
  HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);

  // 3
  GPIO_PinState pinState;
  if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
      for(;;){}
  }
  if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 4
  GPIO_InitStruct.Pin = SDA_PIN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_TogglePin(GPIOB, SDA_PIN);

  // 5
  if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_SET) {
      for(;;){}
  }

  // 6
  GPIO_InitStruct.Pin = SCL_PIN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_TogglePin(GPIOB, SCL_PIN);

  // 7
  if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_SET) {
      for(;;){}
  }

  // 8
  GPIO_InitStruct.Pin = SDA_PIN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
  TIM_delayMs(1);

  // 9
  if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 10
  GPIO_InitStruct.Pin = SCL_PIN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);
  TIM_delayMs(1);

  // 11
  if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 12
  GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Alternate = h->hw->io_cfg_ext.af_value;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // 13
  hi2c->Instance->CR1 |= I2C_CR1_SWRST;

  // 14
  hi2c->Instance->CR1 ^= I2C_CR1_SWRST;

  // 15
  __HAL_I2C_ENABLE(hi2c);

}
