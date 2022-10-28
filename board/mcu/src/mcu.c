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


/**
 * @file mcu.c
 * @author Rick Davies (richvies@gmail.com)
 * @brief
 * Defines io & peripheral info from mcu.h and mcu_private.h
 * @version 0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */


#include "mcu.h"
#include "mcu_private.h"


/* Board layout configuration */
#define I2C_1_SCL             PORT_PIN_TO_IO(IO_port_B, 6)
#define I2C_1_SDA             PORT_PIN_TO_IO(IO_port_B, 7)
#define I2C_1_PRIORITY        priority_MEDIUM
#define I2C_1_RX_DMA_STREAM   DMA_1_STREAM_0
#define I2C_1_RX_DMA_CH       DMA_CH_1
#define I2C_1_TX_DMA_STREAM   DMA_1_STREAM_6
#define I2C_1_TX_DMA_CH       DMA_CH_1

#define SPI_1_MOSI            PORT_PIN_TO_IO(IO_port_A, 7)
#define SPI_1_MISO            PORT_PIN_TO_IO(IO_port_A, 6)
#define SPI_1_CLK             PORT_PIN_TO_IO(IO_port_A, 5)
#define SPI_1_PRIORITY        priority_MEDIUM
#define SPI_1_RX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_RX_DMA_CH       DMA_CH_3
#define SPI_1_TX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_TX_DMA_CH       DMA_CH_3

#define IO_EXT_1_PIN          (1)

#define ADC_1_DMA_STREAM      DMA_1_STREAM_0


/* IO Ports */
IO_port_hw_info_t const io_ports_hw_info[IO_NUM_OF_PORT] =
{
  /* NULL port for NULL pins */
  {0, NULL},

  {periph_GPIO_A, GPIOA},
  {periph_GPIO_B, GPIOB},
  {periph_GPIO_C, GPIOC},
};

/* IO External interrupt */
io_ext_irq_hw_info_t const io_ext_irq_hw_info[IO_NUM_OF_EXT_IRQ] =
{
  {IO_EXT_1_PIN, EXTI1_IRQn, priority_MEDIUM},
};


/* DMA */
dma_hw_info_t const dma_hw_info[DMA_NUM_OF_STREAM] =
{
  /* DMA_STREAM_NONE: NULL stream to signify not using dma */
  {0, NULL, 0},

  {periph_DMA_1, DMA1_Stream0, DMA1_Stream0_IRQn},
  {periph_DMA_1, DMA1_Stream6, DMA1_Stream6_IRQn},
  {periph_DMA_2, DMA2_Stream0, DMA2_Stream0_IRQn},
  {periph_DMA_2, DMA2_Stream3, DMA2_Stream3_IRQn},
};


/* I2C */
i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH] =
{
  {
    periph_I2C_1,
    I2C1,
    I2C_1_SDA,
    I2C_1_SCL,
    {GPIO_AF4_I2C1},
    I2C1_EV_IRQn,
    I2C1_ER_IRQn,
    I2C_1_PRIORITY,
    I2C_1_TX_DMA_STREAM,
    I2C_1_TX_DMA_CH,
    I2C_1_RX_DMA_STREAM,
    I2C_1_RX_DMA_CH
  },
};


/* SPI */
spi_hw_info_t const spi_hw_info[SPI_NUM_OF_CH] =
{
  {
    periph_SPI_1,
    SPI1,
    SPI_1_MISO,
    SPI_1_MOSI,
    SPI_1_CLK,
    {GPIO_AF5_SPI1},
    SPI1_IRQn,
    SPI_1_PRIORITY,
    SPI_1_TX_DMA_STREAM,
    SPI_1_TX_DMA_CH,
    SPI_1_RX_DMA_STREAM,
    SPI_1_RX_DMA_CH
  },
};
