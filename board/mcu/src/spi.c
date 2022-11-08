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
  DIR_WRITE_READ,
} dir_e;


typedef struct
{
  /// direction of transfer
  dir_e       dir;
  /// chip select pin for device on bus
  IO_num_e    cs_pin;
  /// data buffer to write from
  uint8_t*    tx_data;
  /// data buffer to read to
  uint8_t*    rx_data;
  /// length in bytes for transaction
  uint16_t    length;
  /// function to call when transaction half or done. Set to NULL if not needed
  SPI_xfer_cb cb;
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
  SPI_HandleTypeDef hal;
  spi_hw_info_t const *hw;

  /* current xfer */
  xfer_info_t *xfer;
  bool volatile busy;
  bool volatile done;
  bool volatile error;

  /* xfer queue*/
  xfer_queue_t queue;
} handle_t;

typedef struct
{
  uint16_t div;
  uint32_t hal;
} prescaler_t;



static handle_t handles[SPI_NUM_OF_CH] = {0};

static uint32_t const master_mode_to_hal[SPI_NUM_OF_MASTER_MODES] =
{
  SPI_MODE_MASTER,
  SPI_MODE_SLAVE,
};
static uint32_t const clk_polarity_to_hal[SPI_NUM_OF_CLK_POLARITY] =
{
  SPI_POLARITY_LOW,
  SPI_POLARITY_HIGH,
};
static uint32_t const clk_phase_to_hal[SPI_NUM_OF_CLK_PHASES] =
{
  SPI_PHASE_1EDGE,
  SPI_PHASE_2EDGE,
};
static uint32_t const   bit_order_to_hal[SPI_NUM_OF_BIT_ORDER] =
{
  SPI_FIRSTBIT_MSB,
  SPI_FIRSTBIT_LSB,
};
static prescaler_t const prescaler[8] =
{
  {2,   SPI_BAUDRATEPRESCALER_2},
  {4,   SPI_BAUDRATEPRESCALER_4},
  {8,   SPI_BAUDRATEPRESCALER_8},
  {16,  SPI_BAUDRATEPRESCALER_16},
  {32,  SPI_BAUDRATEPRESCALER_32},
  {64,  SPI_BAUDRATEPRESCALER_64},
  {128, SPI_BAUDRATEPRESCALER_128},
  {256, SPI_BAUDRATEPRESCALER_256},
};


static bool xferBlocking      (dir_e dir,
                               SPI_ch_e ch,
                               IO_num_e cs_pin,
                               uint8_t *tx_data,
                               uint8_t *rx_data,
                               uint16_t len);

static bool xferNonblocking   (dir_e dir,
                               SPI_ch_e ch,
                               IO_num_e cs_pin,
                               uint8_t *tx_data,
                               uint8_t *rx_data,
                               uint16_t len,
                               SPI_xfer_cb cb,
                               void *ctx);
static void xferHandleQ       (handle_t *h);
static bool xferStartNext     (handle_t *h);

static bool writeBlocking     (handle_t *h, uint8_t *data, uint16_t len);
static bool readBlocking      (handle_t *h, uint8_t *data, uint16_t len);
static bool writeReadBlocking (handle_t *h, uint8_t *tx, uint8_t *rx, uint16_t len);

static bool writeIrq          (handle_t *h);
static bool readIrq           (handle_t *h);
static bool writeReadIrq      (handle_t *h);
static bool writeDma          (handle_t *h);
static bool readDma           (handle_t *h);
static bool writeReadDma      (handle_t *h);

static bool     channelFromHal(SPI_HandleTypeDef *hspi, SPI_ch_e *ch);
static void     configureHal(handle_t *h, SPI_cfg_t *cfg);
static uint32_t hzToPrescaler(handle_t *h, uint32_t hz);
static bool     initDma(handle_t *h);


bool SPI_init       (SPI_ch_e ch, SPI_cfg_t *cfg)
{
  bool ret = false;
  handle_t *h;

  if (ch >= SPI_NUM_OF_CH) {
    return false; }

  h = &handles[ch];

  if (h->init) {
    return true; }

  /* link to hw information */
  h->hw = &spi_hw_info[ch];

  /* set config params */
  configureHal(h, cfg);

  if(HAL_OK == HAL_SPI_Init(&h->hal))
  {
    /* point irq to this handle (h) for use in interrupt handler */
    irq_set_context(h->hw->irq_num, h);

    ret = true;
  }
  else
  {
    MERR_error(MERROR_SPI_INIT, h->hw->periph);
  }

  h->init = ret;

  return ret;
}

bool SPI_deInit     (SPI_ch_e ch)
{
  bool ret = false;
  handle_t *h;

  if (ch >= SPI_NUM_OF_CH) {
    return false; }

  h = &handles[ch];

  if (HAL_OK == HAL_SPI_DeInit(&h->hal))
  {
    h->init = false;
    ret = true;

    if (h->hw->dma_rx_stream) {
      dma_deinit(h->hw->dma_rx_stream); }

    if (h->hw->dma_tx_stream) {
      dma_deinit(h->hw->dma_tx_stream); }
  }

  return ret;
}

void SPI_task       (void)
{
  SPI_ch_e ch;
  handle_t *h;

  /* handle xfer queue of each channel */
  for (ch = SPI_CH_FIRST; ch < SPI_NUM_OF_CH; ch++)
  {
    h = &handles[ch];

    if (h->done) {
      xferHandleQ(h); }
  }
}

bool SPI_write      (SPI_ch_e    ch,
                     IO_num_e    cs_pin,
                     uint8_t*    tx_data,
                     uint16_t    length,
                     SPI_xfer_cb cb,
                     void*       ctx)
{
  return xferNonblocking(DIR_WRITE, ch, cs_pin, tx_data, NULL, length, cb, ctx);
}

bool SPI_read       (SPI_ch_e    ch,
                     IO_num_e    cs_pin,
                     uint8_t*    rx_data,
                     uint16_t    length,
                     SPI_xfer_cb cb,
                     void*       ctx)
{
  return xferNonblocking(DIR_READ, ch, cs_pin, NULL, rx_data, length, cb, ctx);
}

bool SPI_writeRead  (SPI_ch_e    ch,
                     IO_num_e    cs_pin,
                     uint8_t*    tx_data,
                     uint8_t*    rx_data,
                     uint16_t    length,
                     SPI_xfer_cb cb,
                     void*       ctx)
{
  return xferNonblocking(DIR_WRITE_READ, ch, cs_pin, tx_data, rx_data, length, cb, ctx);
}

bool SPI_writeBlocking (SPI_ch_e    ch,
                        IO_num_e    cs_pin,
                        uint8_t*    tx_data,
                        uint16_t    length)
{
  return xferBlocking(DIR_WRITE, ch, cs_pin, tx_data, NULL, length);
}

bool SPI_readBlocking (SPI_ch_e    ch,
                       IO_num_e    cs_pin,
                       uint8_t*    rx_data,
                       uint16_t    length)
{
  return xferBlocking(DIR_READ, ch, cs_pin, NULL, rx_data, length);
}

bool SPI_writeReadBlocking  (SPI_ch_e    ch,
                             IO_num_e    cs_pin,
                             uint8_t*    tx_data,
                             uint8_t*    rx_data,
                             uint16_t    length)
{
  return xferBlocking(DIR_WRITE_READ, ch, cs_pin, tx_data, rx_data, length);
}


/* Transfer functions */

static bool xferBlocking  (dir_e dir,
                           SPI_ch_e ch,
                           IO_num_e cs_pin,
                           uint8_t *tx_data,
                           uint8_t *rx_data,
                           uint16_t len)
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
              MERR_error(MERROR_SPI_BUSY_TIMEOUT, h->hw->periph);
              return false;
            });
  }

  if (cs_pin) {
    IO_clear(cs_pin); }

  switch (dir)
  {
  case DIR_WRITE:
      ret = writeBlocking(h, tx_data, len);
    break;
  case DIR_READ:
      ret = readBlocking(h, rx_data, len);
    break;

  case DIR_WRITE_READ:
      ret = writeReadBlocking(h, tx_data, rx_data, len);
    break;

  default:
    break;
  }

  if (cs_pin) {
    IO_set(cs_pin); }

  if (false == ret) {
    MERR_error(MERROR_SPI_XFER_START, h->hw->periph); }

  return ret;
}

static bool xferNonblocking (dir_e dir,
                             SPI_ch_e ch,
                             IO_num_e cs_pin,
                             uint8_t *tx_data,
                             uint8_t *rx_data,
                             uint16_t len,
                             SPI_xfer_cb cb,
                             void *ctx)
{
  bool ret = false;
  handle_t *h = &handles[ch];
  xfer_info_t *info;

  /* add to q and try start next xfer */

  if (Q_isFull(h->queue)) {
    MERR_error(MERROR_SPI_XFER_Q_OVERFLOW, h->hw->periph); }
  else
  {
    ret = true;

    info = &Q_getHead(h->queue);

    info->dir = dir;
    info->cs_pin = cs_pin;
    info->tx_data = tx_data;
    info->rx_data = rx_data;
    info->cb = cb;
    info->ctx = ctx;
    info->length = len;

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
      MERR_error(MERROR_SPI_XFER_ERROR, h->hw->periph); }

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
        h->xfer->cb(true, true, h->xfer->ctx); }

      Q_incTail(h->queue);
    }
  }
}

static bool xferStartNext   (handle_t *h)
{
  bool ret = false;

  h->xfer = &Q_getTail(h->queue);

  if (h->xfer->cs_pin) {
    IO_clear(h->xfer->cs_pin); }

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

  case DIR_WRITE_READ:
    if (h->hw->dma_tx_stream && h->hw->dma_rx_stream) {
      ret = writeReadDma(h); }
    else {
      ret = writeReadIrq(h); }
    break;

  default:
    break;
  }

  /* busy if xfer started successfully */
  h->busy = ret;

  if (false == ret)
  {
    if (h->xfer->cs_pin) {
      IO_set(h->xfer->cs_pin); }

    MERR_error(MERROR_SPI_XFER_START, h->hw->periph);
  }

  return ret;
}


/* Blocking low level */

static bool writeBlocking     (handle_t *h, uint8_t *data, uint16_t len)
{
  return (HAL_OK == HAL_SPI_Transmit(&h->hal,
                                     data,
                                     len,
                                     100));
}

static bool readBlocking      (handle_t *h, uint8_t *data, uint16_t len)
{
  return (HAL_OK == HAL_SPI_Receive(&h->hal,
                                     data,
                                     len,
                                     100));
}

static bool writeReadBlocking (handle_t *h, uint8_t *tx, uint8_t *rx, uint16_t len)
{
  return (HAL_OK == HAL_SPI_TransmitReceive(&h->hal,
                                             tx,
                                             rx,
                                             len,
                                             100));
}

/* Non-Blocking low level */

static bool writeIrq      (handle_t *h)
{
  return (HAL_OK == HAL_SPI_Transmit_IT(&h->hal,
                                         h->xfer->tx_data,
                                         h->xfer->length));
}

static bool readIrq       (handle_t *h)
{
  return (HAL_OK == HAL_SPI_Receive_IT(&h->hal,
                                        h->xfer->rx_data,
                                        h->xfer->length));
}

static bool writeReadIrq  (handle_t *h)
{
  return (HAL_OK == HAL_SPI_TransmitReceive_IT(&h->hal,
                                                h->xfer->tx_data,
                                                h->xfer->rx_data,
                                                h->xfer->length));
}

static bool writeDma      (handle_t *h)
{
  return (HAL_OK == HAL_SPI_Transmit_DMA(&h->hal,
                                          h->xfer->tx_data,
                                          h->xfer->length));
}

static bool readDma       (handle_t *h)
{
  return (HAL_OK == HAL_SPI_Receive_DMA(&h->hal,
                                        h->xfer->rx_data,
                                        h->xfer->length));
}

static bool writeReadDma  (handle_t *h)
{
  return (HAL_OK == HAL_SPI_TransmitReceive_DMA(&h->hal,
                                                 h->xfer->tx_data,
                                                 h->xfer->rx_data,
                                                 h->xfer->length));
}


/* Util functions */

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

static void configureHal(handle_t *h, SPI_cfg_t *cfg)
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

static uint32_t hzToPrescaler(handle_t *h, uint32_t hz)
{
  uint8_t i = 0;
  uint32_t clk_hz = clk_getPeriphBaseClkHz(h->hw->periph);

  /* search for clock speed, equal to or lower than requested speed */
  while (i < SIZEOF(prescaler))
  {
    if ((clk_hz / prescaler[i].div) <= hz)
    {
      break;
    }

    ++i;
  }

  return prescaler[i].hal;
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

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  SPI_ch_e ch;
  handle_t *h;
  IO_cfg_t io_cfg;

  if (false == channelFromHal(hspi, &ch)) {
    return; }

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
  handle_t *h;

  if (false == channelFromHal(hspi, &ch))
    return;

  h  = &handles[ch];

  irq_disable(h->hw->irq_num);

  IO_deinit(h->hw->sck_pin);
  IO_deinit(h->hw->miso_pin);
  IO_deinit(h->hw->mosi_pin);

  clk_periphReset(h->hw->periph);
}


/* Interrupt handling */

void spi_irq_handler(void)
{
  handle_t *h = (handle_t*)irq_get_context(irq_get_current());

  if (h) {
    HAL_SPI_IRQHandler(&h->hal); }
}

static void irq_end_callback(bool error, bool done)
{
  handle_t *h = (handle_t*)irq_get_context(irq_get_current());

  if (h)
  {
    h->error = error;

    if (done)
    {
      h->done = true;
      if (h->xfer->cs_pin) {
        IO_set(h->xfer->cs_pin); }
    }

    if (h->xfer->cb) {
      h->xfer->cb(done, error, h->xfer->ctx); }
  }
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, true);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, true);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, true);
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, false);
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, false);
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(false, false);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(true, true);
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
  irq_end_callback(true, true);
}
