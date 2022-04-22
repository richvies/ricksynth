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


#ifndef __BOARD_H
#define __BOARD_H


#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"


// DBG_Printf(INFO_LVL, RAD_ID, x, ##__VA_ARGS__)
#define PRINTF_VERB(x, ...)
#define PRINTF_INFO(x, ...)
#define PRINTF_WARN(x, ...)
#define PRINTF_ERRO(x, ...)


/* IO Pins */
typedef uint16_t IO_num_e;

extern IO_num_e const CHIP_spi_nss_pin;
extern IO_num_e const IO_pin_builtin_led;

/* IO External interrupt */
typedef enum
{
  IO_EXT_IRQ_FIRST = 0,
  IO_EXT_IRQ_1 = IO_EXT_IRQ_FIRST,
  IO_NUM_OF_EXT_IRQ,
} IO_ext_irq_e;


/* DMA */
typedef enum
{
  DMA_STREAM_NONE  = 0,
  DMA_STREAM_FIRST = 1,
  DMA_1_STREAM_0 = DMA_STREAM_FIRST,
  DMA_1_STREAM_6,
  DMA_NUM_OF_STREAM,
} DMA_stream_e;

typedef enum
{
  DMA_CH_FIRST = 0,
  DMA_CH_0 = DMA_CH_FIRST,
  DMA_CH_1,
  DMA_CH_2,
  DMA_CH_3,
  DMA_CH_4,
  DMA_CH_5,
  DMA_CH_6,
  DMA_CH_7,
  DMA_NUM_OF_CH,
} DMA_ch_e;


/* I2C */
typedef enum
{
  I2C_CH_FIRST = 0,
  I2C_CH_1 = I2C_CH_FIRST,
  I2C_NUM_OF_CH,
} I2C_ch_e;


/* Timers */
typedef enum
{
  TIM_CH_1,
  TIM_CH_2,
  TIM_CH_3,
  TIM_CH_4,
  TIM_CH_5,
  TIM_CH_9,
  TIM_CH_10,
  TIM_CH_11,
  TIM_NUM_OF_CH,
} TIM_ch_e;

typedef enum
{
  TIM_SUB_CH_1,
  TIM_SUB_CH_2,
  TIM_SUB_CH_3,
  TIM_SUB_CH_4,
  TIM_NUM_OF_SUB_CH,
} TIM_sub_ch_e;

typedef enum
{
  ALARM_CH_1,
  ALARM_CH_2,
  ALARM_CH_3,
  ALARM_CH_4,
  ALARM_NUM_OF_CH,
} ALARM_ch_e;


#endif
