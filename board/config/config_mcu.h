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


#ifndef __CONFIG_MCU_H
#define __CONFIG_MCU_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "common.h"


/* IO */
typedef enum
{
  IO_port_NULL,
  IO_port_A,
  IO_port_B,
  IO_port_C,
  IO_NUM_OF_PORT,
} IO_port_e;

/* IO External interrupt */
typedef enum
{
  IO_EXT_IRQ_FIRST = 0,
  IO_EXT_IRQ_1 = IO_EXT_IRQ_FIRST,
  IO_NUM_OF_EXT_IRQ,
} IO_ext_irq_e;


/* I2C */
typedef enum
{
  I2C_CH_FIRST = 0,
  I2C_CH_1 = I2C_CH_FIRST,
  I2C_NUM_OF_CH,
} I2C_ch_e;


/* DMA */
typedef enum
{
  DMA_STREAM_NONE  = 0,
  DMA_STREAM_FIRST = 1,
  DMA_1_STREAM_0 = DMA_STREAM_FIRST,
  DMA_1_STREAM_6,
  DMA_2_STREAM_0,
  DMA_2_STREAM_3,
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


/* SPI */
typedef enum
{
  SPI_CH_FIRST = 0,
  SPI_CH_1 = SPI_CH_FIRST,
  SPI_NUM_OF_CH,
} SPI_ch_e;


/* Timers */
typedef enum
{
  TIM_CH_FIRST,
  TIM_CH_1 = TIM_CH_FIRST,
  TIM_NUM_OF_CH,
} TIM_ch_e;

typedef enum
{
  TIM_SUB_CH_FIRST,
  TIM_SUB_CH_1 = TIM_SUB_CH_FIRST,
  TIM_NUM_OF_SUB_CH,
} TIM_sub_ch_e;

typedef enum
{
  ALARM_CH_FIRST,
  ALARM_CH_1 = ALARM_CH_FIRST,
  ALARM_NUM_OF_CH,
} ALARM_ch_e;


/* ADC */
typedef enum
{
  ADC_CH_FIRST,
  ADC_1_CH_1 = ADC_CH_FIRST,
  ADC_NUM_OF_CH,
} ADC_ch_e;

#define ADC_NUM_OF_PERIPH           (1)


#ifdef __cplusplus
}
#endif


#endif
