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
