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


#ifndef __HW_INFO_H
#define __HW_INFO_H


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

#include "io.h"
#include "irq.h"


#define IO_TO_PIN(io)               ((uint16_t)(1 << ((uint8_t)io)))
#define IO_TO_PORT(io)              ((uint8_t)(io >> 8))
#define IO_TO_GPIO_INST(io)         (io_ports_hw_info[IO_TO_PORT(io)].inst)


typedef struct
{
  uint8_t const af_value;
} io_cfg_extend_t;

typedef struct
{
  periph_e       const periph;
  GPIO_TypeDef * const inst;
} io_port_hw_info_t;


/* IO external interrupt */
typedef struct
{
  IO_num_e       const io_num;
  irq_num_e      const irq_num;
  irq_priority_e const priority;
} io_ext_irq_hw_info_t;


/* DMA */
typedef struct
{
  periph_e            const periph;
  DMA_Stream_TypeDef* const inst;
  irq_num_e           const irq_num;
} dma_hw_info_t;


/* USART */
typedef struct
{
  periph_e              const periph;
  USART_TypeDef *       const inst;
  IO_num_e              const tx_pin;
  IO_num_e              const rx_pin;
  IO_num_e              const rts_pin;
  IO_num_e              const cts_pin;
  io_cfg_extend_t       const io_cfg_ext;
  irq_num_e             const irq_num;
  irq_priority_e        const irq_priority;
  DMA_stream_e          const dma_tx_stream;
  DMA_ch_e              const dma_tx_ch;
  DMA_stream_e          const dma_rx_stream;
  DMA_ch_e              const dma_rx_ch;
} usart_hw_info_t;


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
  DMA_stream_e          const dma_tx_stream;
  DMA_ch_e              const dma_tx_ch;
  DMA_stream_e          const dma_rx_stream;
  DMA_ch_e              const dma_rx_ch;
} spi_hw_info_t;


/* Timer */
typedef struct
{
  periph_e        const periph;
  TIM_TypeDef *   const inst;
} tim_hw_info_t;


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
  DMA_stream_e    const dma_stream;
  DMA_ch_e        const dma_ch;
} adc_hw_info_t;


extern io_port_hw_info_t    const io_ports_hw_info[IO_NUM_OF_PORT];
extern io_ext_irq_hw_info_t const io_ext_irq_hw_info[IO_NUM_OF_EXT_IRQ];
extern dma_hw_info_t        const dma_hw_info[DMA_NUM_OF_STREAM];
extern i2c_hw_info_t        const i2c_hw_info[I2C_NUM_OF_CH];
extern spi_hw_info_t        const spi_hw_info[SPI_NUM_OF_CH];
extern tim_hw_info_t        const tim_hw_info[TIM_NUM_OF_CH];
extern adc_hw_info_t        const adc_hw_info[ADC_NUM_OF_PERIPH];
extern adc_ch_info_t        const adc_ch_info[ADC_NUM_OF_CH];


#endif
