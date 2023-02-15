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


#ifdef I2C_USED


#include "_hw_info.h"

#include "common.h"

#include "clk.h"
#include "dma.h"
#include "io.h"
#include "irq.h"
#include "merror.h"
#include "tim.h"


typedef enum
{
  DIR_WRITE,
  DIR_READ,
  DIR_READ_MEM,
} dir_e;


typedef struct
{
  /// direction of transfer
  dir_e       dir;
  /// address of device on bus
  uint16_t    addr;
  /// memory register to read
  uint16_t    mem_addr;
  /// length of register address
  uint16_t    mem_length;
  /// data buffer to write/ read
  uint8_t*    data;
  /// length in bytes for write/ read
  uint16_t    length;
  /// function to call when transaction half or done. Set to NULL if not needed
  I2C_xfer_cb cb;
  /// will be passed as argument to cb function
  void*       ctx;
} xfer_info_t;

typedef struct
{
  xfer_info_t buf[5];
  uint8_t count;
  uint8_t tail;
} xfer_queue_t;

typedef struct
{
  bool init;
  I2C_HandleTypeDef hal;
  i2c_hw_info_t const *hw;

  /* current xfer */
  xfer_info_t *xfer;
  bool volatile busy;
  bool volatile done;
  bool volatile error;

  /* xfer queue*/
  xfer_queue_t queue;
} handle_t;


static handle_t handles[I2C_NUM_OF_CH] = {0};

static uint32_t const address_mode_to_hal[I2C_NUM_OF_ADDRESS_MODES] =
{
  I2C_ADDRESSINGMODE_7BIT,
  I2C_ADDRESSINGMODE_10BIT,
};
static uint32_t const stretch_mode_to_hal[I2C_NUM_OF_STRETCH_MODES] =
{
  I2C_NOSTRETCH_ENABLE,
  I2C_NOSTRETCH_DISABLE,
};


static bool xferBlocking  (dir_e       dir,
                           I2C_ch_e    ch,
                           uint16_t    addr,
                           uint16_t    mem_addr,
                           uint8_t     mem_length,
                           uint8_t*    data,
                           uint16_t    length);

static bool xferNonblocking   (dir_e        dir,
                               I2C_ch_e     ch,
                               uint16_t     addr,
                               uint16_t     mem_addr,
                               uint8_t      mem_length,
                               uint8_t*     data,
                               uint16_t     length,
                               I2C_xfer_cb  cb,
                               void*        ctx);

static void xferHandleQ       (handle_t *h);
static bool xferStartNext     (handle_t *h);

static bool writeBlocking   (handle_t *h, uint16_t addr, uint8_t *data, uint16_t len);
static bool readBlocking    (handle_t *h, uint16_t addr, uint8_t *data, uint16_t len);
static bool readMemBlocking (handle_t *h, uint16_t addr, uint16_t mem_addr, uint16_t mem_addr_length, uint8_t *data, uint16_t length);

static bool writeIrq          (handle_t *h);
static bool readIrq           (handle_t *h);
static bool readMemIrq        (handle_t *h);
static bool writeDma          (handle_t *h);
static bool readDma           (handle_t *h);
static bool readMemDma        (handle_t *h);


static bool channelFromHal(I2C_HandleTypeDef *hi2c, I2C_ch_e *ch);
static void configureHal(handle_t *h, I2C_cfg_t *cfg);
static bool initDma(handle_t *h);

/* required to unstick gpio pins used with i2c peripheral */
static void HAL_I2C_ClearBusyFlagErrata_2_14_7(handle_t *h);


bool I2C_init       (I2C_ch_e ch, I2C_cfg_t *cfg)
{
  bool ret = false;
  handle_t *h;

  if (ch >= I2C_NUM_OF_CH) {
    return false; }

  h = &handles[ch];

  if (true == h->init) {
    return true; }

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
  else
  {
    MERR_error(MERROR_I2C_INIT, h->hw->periph);
  }

  h->init = ret;

  return ret;
}

bool I2C_deInit     (I2C_ch_e ch)
{
  bool ret = false;
  handle_t *h;

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

void I2C_task       (void)
{
  I2C_ch_e ch;
  handle_t *h;

  /* handle xfer queue of each channel */
  for (ch = I2C_CH_FIRST; ch < I2C_NUM_OF_CH; ch++)
  {
    h = &handles[ch];

    if (h->done) {
      xferHandleQ(h); }
  }
}

bool I2C_write      (I2C_ch_e    ch,
                     uint16_t    addr,
                     uint8_t*    tx_data,
                     uint16_t    length,
                     I2C_xfer_cb cb,
                     void*       ctx)
{
  return xferNonblocking(DIR_WRITE, ch, addr, 0, 0, tx_data, length, cb, ctx);
}

bool I2C_read       (I2C_ch_e    ch,
                     uint16_t    addr,
                     uint8_t*    rx_data,
                     uint16_t    length,
                     I2C_xfer_cb cb,
                     void*       ctx)
{
  return xferNonblocking(DIR_READ, ch, addr, 0, 0, rx_data, length, cb, ctx);
}

bool I2C_readMem    (I2C_ch_e     ch,
                     uint16_t     addr,
                     uint16_t     mem_addr,
                     uint8_t      mem_length,
                     uint8_t*     rx_data,
                     uint16_t     length,
                     I2C_xfer_cb  cb,
                     void*        ctx)
{
  return xferNonblocking(DIR_READ_MEM, ch, addr, mem_addr, mem_length, rx_data, length, cb, ctx);
}

bool I2C_writeBlocking    (I2C_ch_e    ch,
                           uint16_t    addr,
                           uint8_t*    tx_data,
                           uint16_t    length)
{
  return xferNonblocking(DIR_WRITE, ch, addr, 0, 0, tx_data, length, NULL, NULL);
}

bool I2C_readBlocking     (I2C_ch_e    ch,
                           uint16_t    addr,
                           uint8_t*    rx_data,
                           uint16_t    length)
{
  return xferNonblocking(DIR_READ, ch, addr, 0, 0, rx_data, length, NULL, NULL);
}

bool I2C_readMemBlocking  (I2C_ch_e     ch,
                           uint16_t     addr,
                           uint16_t     mem_addr,
                           uint8_t      mem_length,
                           uint8_t*     rx_data,
                           uint16_t     length)
{
  return xferNonblocking(DIR_READ_MEM, ch, addr, mem_addr, mem_length, rx_data, length, NULL, NULL);
}


/* Transfer functions */

static bool xferBlocking  (dir_e       dir,
                           I2C_ch_e    ch,
                           uint16_t    addr,
                           uint16_t    mem_addr,
                           uint8_t     mem_length,
                           uint8_t*    data,
                           uint16_t    length)
{
  bool ret = false;
  handle_t *h = &handles[ch];

  /* if busy - wait until current xfer done */
  if (h->busy)
  {
    TIMEOUT(false == h->done,
            100,
            EMPTY,
            EMPTY,
            {
              MERR_error(MERROR_I2C_BUSY_TIMEOUT, h->hw->periph);
              return false;
            });
  }

  switch (dir)
  {
  case DIR_WRITE:
      ret = writeBlocking(h, addr, data, length);
    break;
  case DIR_READ:
      ret = readBlocking(h, addr, data, length);
    break;

  case DIR_READ_MEM:
      ret = readMemBlocking(h, addr, mem_addr, mem_length, data, length);
    break;

  default:
    break;
  }

  if (false == ret) {
    MERR_error(MERROR_I2C_XFER_START, h->hw->periph); }

  return ret;
}

static bool xferNonblocking (dir_e        dir,
                             I2C_ch_e     ch,
                             uint16_t     addr,
                             uint16_t     mem_addr,
                             uint8_t      mem_length,
                             uint8_t*     data,
                             uint16_t     length,
                             I2C_xfer_cb  cb,
                             void*        ctx)
{
  bool ret = false;
  handle_t *h = &handles[ch];
  xfer_info_t *info;

  /* add to q and try start next xfer */

  if (Q_isFull(h->queue)) {
    MERR_error(MERROR_I2C_XFER_Q_OVERFLOW, h->hw->periph); }
  else
  {
    ret = true;

    info = &Q_getHead(h->queue);

    info->dir = dir;
    info->addr = addr;
    info->mem_addr = mem_addr;
    info->mem_length = mem_length;
    info->data = data;
    info->length = length;
    info->cb = cb;
    info->ctx = ctx;

    Q_incHead(h->queue);

    xferHandleQ(h);
  }

  return ret;
}

static void xferHandleQ   (handle_t *h)
{
  /* if current xfer done -
  * record error, update queue, reset for next xfer */
  if (h->done)
  {
    if (h->error) {
      MERR_error(MERROR_I2C_XFER_ERROR, h->hw->periph); }

    Q_incTail(h->queue);

    h->busy = false;
    h->done = false;
    h->error = false;
  }

  /* try start next xfer, if fails - callback with error and try next... */
  if (false == h->busy)
  {
    while ((true == Q_pending(h->queue)) && (false == xferStartNext(h)))
    {
      if (h->xfer->cb) {
        h->xfer->cb(true, h->xfer->ctx); }

      Q_incTail(h->queue);
    }
  }
}

static bool xferStartNext   (handle_t *h)
{
  bool ret = false;

  h->xfer = &Q_getTail(h->queue);

  switch (h->xfer->dir)
  {
  case DIR_WRITE:
    if (h->hw->dma_tx_stream) {
      ret = writeDma(h); }
    else {
      ret = writeIrq(h); }
    break;

  case DIR_READ:
    if (h->hw->dma_rx_stream) {
      ret = readDma(h); }
    else {
      ret = readIrq(h); }
    break;

  case DIR_READ_MEM:
    if (h->hw->dma_tx_stream && h->hw->dma_rx_stream) {
      ret = readMemDma(h); }
    else {
      ret = readMemIrq(h); }
    break;

  default:
    break;
  }

  /* busy if xfer started successfully */
  h->busy = ret;

  if (false == ret)
  {
    MERR_error(MERROR_I2C_XFER_START, h->hw->periph);
  }

  return ret;
}


/* Blocking low level */

static bool writeBlocking   (handle_t *h, uint16_t addr, uint8_t *data, uint16_t len)
{
  return (HAL_OK == HAL_I2C_Master_Transmit(&h->hal,
                                            addr,
                                            data,
                                            len,
                                            100));
}

static bool readBlocking    (handle_t *h, uint16_t addr, uint8_t *data, uint16_t len)
{
  return (HAL_OK == HAL_I2C_Master_Receive(&h->hal,
                                           addr,
                                           data,
                                           len,
                                           100));
}

static bool readMemBlocking (handle_t *h, uint16_t addr, uint16_t mem_addr, uint16_t mem_addr_length, uint8_t *data, uint16_t length)
{
  return (HAL_OK == HAL_I2C_Mem_Read(&h->hal,
                                     addr,
                                     mem_addr,
                                     mem_addr_length,
                                     data,
                                     length,
                                     100));
}


/* Non-Blocking low level */

static bool writeIrq    (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_IT(
    &h->hal, h->xfer->addr, h->xfer->data, h->xfer->length));
}

static bool readIrq     (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Master_Receive_IT(
    &h->hal, h->xfer->addr, h->xfer->data, h->xfer->length));
}

static bool readMemIrq  (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Mem_Read_IT(&h->hal,
                                          h->xfer->mem_addr,
                                          h->xfer->mem_length,
                                          h->xfer->addr,
                                          h->xfer->data,
                                          h->xfer->length));
}

static bool writeDma    (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Master_Transmit_DMA(
    &h->hal, h->xfer->addr, h->xfer->data, h->xfer->length));
}

static bool readDma     (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Master_Receive_DMA(
    &h->hal, h->xfer->addr, h->xfer->data, h->xfer->length));
}

static bool readMemDma  (handle_t *h)
{
  return (HAL_OK == HAL_I2C_Mem_Read_DMA(&h->hal,
                                          h->xfer->mem_addr,
                                          h->xfer->mem_length,
                                          h->xfer->addr,
                                          h->xfer->data,
                                          h->xfer->length));
}


/* Util functions */

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

static void configureHal(handle_t *h, I2C_cfg_t *cfg)
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

static bool initDma(handle_t *h)
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


/* STM32 Library functions */

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  I2C_ch_e ch;
  handle_t *h;
  IO_cfg_t io_cfg;

  if (false == channelFromHal(hi2c, &ch)) {
    return; }

  h  = &handles[ch];

  io_cfg.mode    = IO_MODE_PERIPH_OUT_OD;
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
  handle_t *h;

  if (false == channelFromHal(hi2c, &ch)) {
    return; }

  h  = &handles[ch];

  irq_disable(h->hw->event_irq_num);
  irq_disable(h->hw->error_irq_num);

  IO_deinit(h->hw->sda_pin);
  IO_deinit(h->hw->scl_pin);

  clk_periphReset(h->hw->periph);
}


/* Interrupt handling */

void i2c_event_irq_handler(void)
{
  handle_t *h = (handle_t*)irq_get_context(irq_get_current());

  if (h) {
    HAL_I2C_EV_IRQHandler(&h->hal); }
}

void i2c_error_irq_handler(void)
{
  handle_t *h = (handle_t*)irq_get_context(irq_get_current());

  if (h) {
    HAL_I2C_ER_IRQHandler(&h->hal); }
}

static void irq_end_call_callback(bool error)
{
  handle_t *h = (handle_t*)irq_get_context(irq_get_current());

  if (h)
  {
    h->error = error;
    h->done = true;

    if (h->xfer->cb) {
      h->xfer->cb(error, h->xfer->ctx); }
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

static void HAL_I2C_ClearBusyFlagErrata_2_14_7(handle_t *h)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  I2C_HandleTypeDef *hi2c = &h->hal;

  GPIO_TypeDef *sda_port = io_ports_hw_info[h->hw->sda_pin >> 8].inst;
  uint32_t sda_pin = (uint32_t)1 << (uint8_t)h->hw->sda_pin;

  GPIO_TypeDef *scl_port = io_ports_hw_info[h->hw->scl_pin >> 8].inst;
  uint32_t scl_pin = (uint32_t)1 << (uint8_t)h->hw->scl_pin;

  // 1
  __HAL_I2C_DISABLE(hi2c);

  // 2
  GPIO_InitStruct.Pin = sda_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(sda_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = scl_pin;
  HAL_GPIO_Init(scl_port, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(sda_port, sda_pin);
  HAL_GPIO_WRITE_ODR(scl_port, scl_pin);

  // 3
  GPIO_PinState pinState;
  if (HAL_GPIO_ReadPin(sda_port, sda_pin) == GPIO_PIN_RESET) {
      for(;;){}
  }
  if (HAL_GPIO_ReadPin(scl_port, scl_pin) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 4
  GPIO_InitStruct.Pin = sda_pin;
  HAL_GPIO_Init(sda_port, &GPIO_InitStruct);

  HAL_GPIO_TogglePin(sda_port, sda_pin);

  // 5
  if (HAL_GPIO_ReadPin(sda_port, sda_pin) == GPIO_PIN_SET) {
      for(;;){}
  }

  // 6
  GPIO_InitStruct.Pin = scl_pin;
  HAL_GPIO_Init(scl_port, &GPIO_InitStruct);

  HAL_GPIO_TogglePin(scl_port, scl_pin);

  // 7
  if (HAL_GPIO_ReadPin(scl_port, scl_pin) == GPIO_PIN_SET) {
      for(;;){}
  }

  // 8
  GPIO_InitStruct.Pin = sda_pin;
  HAL_GPIO_Init(sda_port, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(sda_port, sda_pin);
  TIM_delayMs(1);

  // 9
  if (HAL_GPIO_ReadPin(sda_port, sda_pin) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 10
  GPIO_InitStruct.Pin = scl_pin;
  HAL_GPIO_Init(scl_port, &GPIO_InitStruct);

  HAL_GPIO_WRITE_ODR(scl_port, scl_pin);
  TIM_delayMs(1);

  // 11
  if (HAL_GPIO_ReadPin(scl_port, scl_pin) == GPIO_PIN_RESET) {
      for(;;){}
  }

  // 12
  GPIO_InitStruct.Pin = sda_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Alternate = h->hw->io_cfg_ext.af_value;
  HAL_GPIO_Init(sda_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = scl_pin;
  HAL_GPIO_Init(scl_port, &GPIO_InitStruct);
  ;
  // 13
  hi2c->Instance->CR1 |= I2C_CR1_SWRST;

  // 14
  hi2c->Instance->CR1 ^= I2C_CR1_SWRST;

  // 15
  __HAL_I2C_ENABLE(hi2c);
}


#endif
