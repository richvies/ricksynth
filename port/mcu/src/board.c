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


#include "board.h"
#include "mcu.h"


/* Board layout configuration */
#define I2C_1_SCL             PORT_PIN_TO_IO(IO_PORT_B, 6)
#define I2C_1_SDA             PORT_PIN_TO_IO(IO_PORT_B, 7)
#define I2C_1_PRIORITY        PRIORITY_MEDIUM
#define I2C_1_RX_DMA_STREAM   DMA_1_STREAM_0
#define I2C_1_RX_DMA_CH       DMA_CH_1
#define I2C_1_TX_DMA_STREAM   DMA_1_STREAM_6
#define I2C_1_TX_DMA_CH       DMA_CH_1

#define SPI_1_MOSI            PORT_PIN_TO_IO(IO_PORT_A, 7)
#define SPI_1_MISO            PORT_PIN_TO_IO(IO_PORT_A, 6)
#define SPI_1_CLK             PORT_PIN_TO_IO(IO_PORT_A, 5)
#define SPI_1_PRIORITY        PRIORITY_MEDIUM
#define SPI_1_RX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_RX_DMA_CH       DMA_CH_3
#define SPI_1_TX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_TX_DMA_CH       DMA_CH_3

#define CHIP_TLC5928_CS_PIN   PORT_PIN_TO_IO(IO_PORT_A, 12)
#define CHIP_IRQ_IO           PORT_PIN_TO_IO(IO_PORT_A, 14)

#define BUILTIN_LED           PORT_PIN_TO_IO(IO_PORT_C, 13)

#define IO_EXT_1_PIN          CHIP_IRQ_IO

#define ADC_1_DMA_STREAM      DMA_1_STREAM_0


/* Link pins to devices */
IO_num_e const TLC5928_spi_nss_pin = CHIP_TLC5928_CS_PIN;
IO_num_e const IO_pin_builtin_led = BUILTIN_LED;

/* IO Ports */
io_port_hw_info_t const io_ports_hw_info[IO_NUM_OF_PORT] =
{
  /* NULL port for NULL pins */
  {0, NULL},

  {PERIPH_GPIO_A, GPIOA},
  {PERIPH_GPIO_B, GPIOB},
  {PERIPH_GPIO_C, GPIOC},
};

/* IO External interrupt */
io_ext_irq_hw_info_t const io_ext_irq_hw_info[IO_NUM_OF_EXT_IRQ] =
{
  {IO_EXT_1_PIN, EXTI1_IRQn, PRIORITY_MEDIUM},
};


/* DMA */
dma_hw_info_t const dma_hw_info[DMA_NUM_OF_STREAM] =
{
  /* DMA_STREAM_NONE: NULL stream to signify not using dma */
  {0, NULL, 0},

  {PERIPH_DMA_1, DMA1_Stream0, DMA1_Stream0_IRQn},
  {PERIPH_DMA_1, DMA1_Stream6, DMA1_Stream6_IRQn},
  {PERIPH_DMA_2, DMA2_Stream0, DMA2_Stream0_IRQn},
  {PERIPH_DMA_2, DMA2_Stream3, DMA2_Stream3_IRQn},
};


/* I2C */
i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH] =
{
  {
    PERIPH_I2C_1,
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
    PERIPH_SPI_1,
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
