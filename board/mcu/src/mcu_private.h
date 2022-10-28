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


#ifndef __MCU_PRIVATE_H
#define __MCU_PRIVATE_H


/**
 * @file mcu_private.h
 * @author Rick Davies (richvies@gmail.com)
 * @brief
 * Defines internal peripheral enums e.g. gpio ports, irq priorities
 * Also declares hardware information e.g. i2c_hw_info which are
 * defined in mcu.c and used in src files i.e. i2c.c
 * @version 0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */


#include "mcu.h"

#include "stm32f4xx_hal.h"


// DBG_Printf(INFO_LVL, RAD_ID, x, ##__VA_ARGS__)
#define PRINTF_VERB(x, ...)
#define PRINTF_INFO(x, ...)
#define PRINTF_WARN(x, ...)
#define PRINTF_ERRO(x, ...)

#define ADC_NUM_OF_PERIPH           (1)

/* Generic data queue handling macros, can use as FIFO or LIFO
 *
 * ATTN: must follow this naming convention
 *
 * buf    - the queue
 * tail   - tail index
 * count  - current count
 *
 *
 * e.g. structure
 * typedef struct
 * {
 *  uint32_t  buf[10];
 *  uint8_t   tail = 0;
 *  uint8_t   count = 0;
 * } some_queue_t;
 *
 *
 * e.g. usage
 *
 * some_queue_t my_q;
 *
 * Q_clear(my_q);
 *
 * // adding items
 * if (false == Q_isFull(my_q))
 * {
 *    Q_getHead(my_q) = 666;
 *    Q_incHead(my_q);
 * }
 *
 * // removing FIFO
 * if (true == Q_pending(my_q))
 * {
 *  uin32_t tmp = Q_getTail(my_q);
 *  Q_incTail(my_q)
 * }
 *
 * // removing LIFO
 * if (true == Q_pending(my_q))
 * {
 *  uin32_t tmp = Q_getHead(my_q);
 *  Q_decHead(my_q)
 * }
 */

#define Q_clear(q)          ( q.tail = 0; q.count = 0; )
#define Q_pending(q)        ({ q.count > 0; })
#define Q_isFull(q)         ({ q.count >= SIZEOF(q.buf); })

#define Q_getHead(q)        ( q.buf[q.tail + q.count] )
#define Q_incHead(q)        { ++q.count; }
#define Q_getTail(q)        ( q.buf[q.tail] )
#define Q_incTail(q)        { --q.count; ++q.tail; if (q.tail >= SIZEOF(q.buf)) {q.tail = 0;} }

/* for LIFO use */
#define Q_decHead(q)        { --q.count; }


typedef IRQn_Type irq_num_e;

typedef enum
{
  priority_VERY_HIGH,
  priority_HIGH,
  priority_MEDIUM,
  priority_LOW,
  priority_VERY_LOW,
  priority_NUM_OF,
} irq_priority_e;


/* for use with clk module */
typedef enum
{
  periph_GPIO_A,
  periph_GPIO_B,
  periph_GPIO_C,
  periph_I2C_1,
  periph_I2C_2,
  periph_I2C_3,
  periph_SPI_1,
  periph_SPI_2,
  periph_SPI_3,
  periph_SPI_4,
  periph_DMA_1,
  periph_DMA_2,
  periph_ADC_1,
} periph_e;


typedef struct
{
  uint8_t const af_value;
} io_cfg_extend_t;

typedef struct
{
  periph_e       const periph;
  GPIO_TypeDef * const inst;
} IO_port_hw_info_t;

extern IO_port_hw_info_t const io_ports_hw_info[IO_NUM_OF_PORT];

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
  dma_stream_e          const dma_tx_stream;
  dma_ch_e              const dma_tx_ch;
  dma_stream_e          const dma_rx_stream;
  dma_ch_e              const dma_rx_ch;
} i2c_hw_info_t;

extern i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH];


/* SPI */
typedef struct
{
  periph_e              const periph;
  SPI_TypeDef *         const inst;
  IO_num_e              const miso_pin;
  IO_num_e              const mosi_pin;
  IO_num_e              const sck_pin;
  io_cfg_extend_t       const io_cfg_ext;
  irq_num_e             const irq_num;
  irq_priority_e        const irq_priority;
  dma_stream_e          const dma_tx_stream;
  dma_ch_e              const dma_tx_ch;
  dma_stream_e          const dma_rx_stream;
  dma_ch_e              const dma_rx_ch;
} spi_hw_info_t;

extern spi_hw_info_t const spi_hw_info[SPI_NUM_OF_CH];


/* Timer */
typedef struct
{
  periph_e        const periph;
  TIM_TypeDef *   const inst;
} tim_hw_info_t;

extern tim_hw_info_t const tim_hw_info[TIM_NUM_OF_CH];


/* ADC */
typedef struct
{
  IO_num_e            const io_pin;
  uint32_t            const channel;
} adc_ch_info_t;

typedef struct
{
  periph_e        const periph;
  ADC_TypeDef *   const inst;
  irq_num_e       const irq_num;
  irq_priority_e  const irq_priority;
  dma_stream_e    const dma_stream;
  dma_ch_e        const dma_ch;
  adc_ch_info_t   const ch_info[ADC_NUM_OF_CH];
} adc_hw_info_t;


extern adc_hw_info_t const adc_hw_info[ADC_NUM_OF_PERIPH];


#endif
