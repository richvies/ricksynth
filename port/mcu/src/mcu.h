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


#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#include "board.h"


#define PORT_PIN_TO_IO(port, pin)   (((IO_num_e)port << 8) | (uint8_t)pin)
#define IO_TO_PIN(io)               ((uint16_t)(1 << ((uint8_t)io)))
#define IO_TO_PORT(io)              ((uint8_t)(io >> 8))
#define IO_TO_GPIO_INST(io)         (io_ports_hw_info[IO_TO_PORT(io)].inst)
#define IO_NULL_PIN                 (0)
#define IRQ_NUM_OF                  (85)


typedef IRQn_Type irq_num_e;

typedef enum
{
  PRIORITY_VERY_HIGH,
  PRIORITY_HIGH,
  PRIORITY_MEDIUM,
  PRIORITY_LOW,
  PRIORITY_VERY_LOW,
  PRIORITY_NUM_OF,
} irq_priority_e;


typedef enum
{
  PERIPH_GPIO_A,
  PERIPH_GPIO_B,
  PERIPH_GPIO_C,
  PERIPH_I2C_1,
  PERIPH_I2C_2,
  PERIPH_I2C_3,
  PERIPH_DMA_1,
  PERIPH_DMA_2,
} periph_e;


/* IO */
typedef enum
{
  IO_PORT_NULL,
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_NUM_OF_PORT,
} io_port_e;

typedef struct
{
  uint8_t const af_value;
} io_cfg_extend_t;

typedef struct
{
  periph_e       const periph;
  GPIO_TypeDef * const inst;
} io_port_hw_info_t;

extern io_port_hw_info_t const io_ports_hw_info[IO_NUM_OF_PORT];

/* IO external interrupt */
typedef struct
{
  IO_num_e       const io_num;
  irq_num_e      const irq_num;
  irq_priority_e const priority;
} io_ext_irq_hw_info_t;

extern io_ext_irq_hw_info_t const io_ext_irq_hw_info[IO_NUM_OF_EXT_IRQ];


/* DMA */
typedef struct
{
  periph_e            const periph;
  DMA_Stream_TypeDef* const inst;
  irq_num_e           const irq_num;
} dma_hw_info_t;

extern dma_hw_info_t const dma_hw_info[DMA_NUM_OF_STREAM];


/* I2C */
typedef struct
{
  periph_e              const periph;
  I2C_TypeDef *         const inst;
  IO_num_e              const sda_pin;
  IO_num_e              const scl_pin;
  io_cfg_extend_t       const io_cfg_ext;
  irq_num_e             const event_irq_num;
  irq_num_e             const error_irq_num;
  irq_priority_e        const irq_priority;
  DMA_stream_e          const dma_tx_stream;
  DMA_ch_e              const dma_tx_ch;
  DMA_stream_e          const dma_rx_stream;
  DMA_ch_e              const dma_rx_ch;
} i2c_hw_info_t;

extern i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH];

/* Timer */
typedef struct
{
  periph_e        const periph;
  TIM_TypeDef *   const inst;
} tim_hw_info_t;

extern tim_hw_info_t const tim_hw_info[TIM_NUM_OF_CH];


#endif
