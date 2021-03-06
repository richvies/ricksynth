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


#ifndef __SPI_H
#define __SPI_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "board.h"


typedef enum
{
  SPI_MASTER_MODE,
  SPI_SLAVE_MODE,
  SPI_NUM_OF_MASTER_MODES,
} SPI_master_mode_e;

typedef enum
{
  SPI_CLK_POLARITY_IDLE_LOW,
  SPI_CLK_POLARITY_IDLE_HIGH,
  SPI_NUM_OF_CLK_POLARITY,
} SPI_clk_polarity_e;

typedef enum
{
  SPI_CLK_PHASE_SAMPLE_FIRST_EDGE,
  SPI_CLK_PHASE_SAMPLE_SECOND_EDGE,
  SPI_NUM_OF_CLK_PHASES,
} SPI_clk_phase_e;

typedef enum
{
  SPI_BIT_ORDER_MSB_FIRST,
  SPI_BIT_ORDER_LSB_FIRST,
  SPI_NUM_OF_BIT_ORDER,
} SPI_bit_order_e;


typedef struct
{
  SPI_master_mode_e   master_mode;
  SPI_clk_polarity_e  clk_polarity;
  SPI_clk_phase_e     clk_phase;
  SPI_bit_order_e     bit_order;
  uint32_t            clk_speed_hz;
} SPI_cfg_t;

typedef void (*SPI_xfer_cb)(bool error, bool done, void *ctx);

typedef struct
{
  IO_num_e    cs_pin;
  uint8_t*    tx_data;
  uint8_t*    rx_data;
  uint16_t    length;
  SPI_xfer_cb cb;
  void*       ctx;
} SPI_xfer_info_t;


extern bool SPI_init      (SPI_ch_e ch, SPI_cfg_t *cfg);
extern bool SPI_deInit    (SPI_ch_e ch);
extern bool SPI_isBusy    (SPI_ch_e ch);
extern bool SPI_write     (SPI_ch_e ch, SPI_xfer_info_t *info);
extern bool SPI_read      (SPI_ch_e ch, SPI_xfer_info_t *info);
extern bool SPI_writeRead (SPI_ch_e ch, SPI_xfer_info_t *info);


#ifdef __cplusplus
}
#endif


#endif
