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


#ifndef __MCU_H
#define __MCU_H


#ifdef __cplusplus
 extern "C" {
#endif


/**
 * @file mcu.h
 * @author Rick Davies (richvies@gmail.com)
 * @brief
 * Defines the mcu peripherals e.g. I2C_CH_2
 * and nunber which are used e.g. I2C_NUM_OF_CH
 * for external use with mcu lib e.g. i2c.h
 * @version 0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */


#include "common.h"

#include "config_mcu.h"


/* IO */
typedef enum
{
  IO_PORT_NULL,
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_H,

  IO_NUM_OF_PORT,
  IO_PORT_FIRST = 1,
} IO_port_e;

/* IO External interrupt */
typedef enum
{
  IO_EXT_IRQ_1,

  IO_NUM_OF_EXT_IRQ,
  IO_EXT_IRQ_FIRST = 0,
} IO_ext_irq_e;

/* USART */
typedef enum
{
  USART_CH_2,

  USART_NUM_OF_CH,
  USART_CH_FIRST = 0,
} USART_ch_e;

/* I2C */
#if (defined I2C_1)
  #define I2C_USED
#endif

typedef enum
{
#ifdef I2C_1
  I2C_CH_1,
#endif

  I2C_NUM_OF_CH,
  I2C_CH_FIRST = 0,
} I2C_ch_e;

/* DMA */
typedef enum
{
  DMA_STREAM_NONE  = 0,
  DMA_1_STREAM_0,
  DMA_1_STREAM_6,
  DMA_2_STREAM_0,
  DMA_2_STREAM_3,

  DMA_NUM_OF_STREAM,
  DMA_STREAM_FIRST = 1,
} DMA_stream_e;

typedef enum
{
  DMA_CH_0,
  DMA_CH_1,
  DMA_CH_2,
  DMA_CH_3,
  DMA_CH_4,
  DMA_CH_5,
  DMA_CH_6,
  DMA_CH_7,

  DMA_NUM_OF_CH,
  DMA_CH_FIRST = 0,
} DMA_ch_e;

/* SPI */
#if (defined SPI_1 || defined SPI_2)
  #define SPI_USED
#endif

typedef enum
{
#ifdef SPI_1
  SPI_CH_1,
#endif

#ifdef SPI_2
  SPI_CH_2,
#endif

#ifdef SPI_3
  SPI_CH_3,
#endif

  SPI_NUM_OF_CH,
  SPI_CH_FIRST = 0,
} SPI_ch_e;

/* Timers */
typedef enum
{
  TIM_CH_1,

  TIM_NUM_OF_CH,
  TIM_CH_FIRST = 0,
} TIM_ch_e;

typedef enum
{
  TIM_SUB_CH_1,
  TIM_SUB_CH_2,
  TIM_SUB_CH_3,
  TIM_SUB_CH_4,

  TIM_NUM_OF_SUB_CH,
  TIM_SUB_CH_FIRST = 0,
} TIM_sub_ch_e;

typedef enum
{
  ALARM_CH_1,

  ALARM_NUM_OF_CH,
  ALARM_CH_FIRST,
} ALARM_ch_e;

/* ADC */
typedef enum
{
  ADC_1_CH_1,

  ADC_NUM_OF_CH,
  ADC_CH_FIRST,
} ADC_ch_e;

/* for use with clk module */
typedef enum
{
  PERIPH_GPIO_A,
  PERIPH_GPIO_B,
  PERIPH_GPIO_C,
  PERIPH_GPIO_H,
  PERIPH_DMA_1,
  PERIPH_DMA_2,
  PERIPH_TIM_5,
  PERIPH_WWDG,
  PERIPH_SPI_2,
  PERIPH_USART_2,
  PERIPH_I2C_1,
  PERIPH_I2C_2,
  PERIPH_PWR,
  PERIPH_TIM_1,
  PERIPH_USART_1,
  PERIPH_USART_6,
  PERIPH_ADC_1,
  PERIPH_SPI_1,
  PERIPH_SYSCFG,
  PERIPH_TIM_9,
  PERIPH_TIM_11,
} PERIPH_e;


#ifdef __cplusplus
}
#endif


#endif
