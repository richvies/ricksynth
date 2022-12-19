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


#include "config_mcu.h"
#include "_hw_info.h"


/* Board layout configuration */
#define USART_2_PRIORITY      PRIORITY_MEDIUM
#define USART_2_RX_DMA_STREAM DMA_STREAM_NONE
#define USART_2_RX_DMA_CH     0
#define USART_2_TX_DMA_STREAM DMA_STREAM_NONE
#define USART_2_TX_DMA_CH     0

#define I2C_1_PRIORITY        PRIORITY_MEDIUM
#define I2C_1_RX_DMA_STREAM   DMA_1_STREAM_0
#define I2C_1_RX_DMA_CH       DMA_CH_1
#define I2C_1_TX_DMA_STREAM   DMA_1_STREAM_6
#define I2C_1_TX_DMA_CH       DMA_CH_1

#define SPI_1_PRIORITY        PRIORITY_MEDIUM
#define SPI_1_RX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_RX_DMA_CH       DMA_CH_3
#define SPI_1_TX_DMA_STREAM   DMA_STREAM_NONE
#define SPI_1_TX_DMA_CH       DMA_CH_3

#define IO_EXT_1_PIN          (1)

#define ADC_1_DMA_STREAM      DMA_1_STREAM_0


/* IO Ports */
io_port_hw_info_t const io_ports_hw_info[IO_NUM_OF_PORT] =
{
  /* periph       inst */

  /* NULL port for NULL pins */
  {0,             NULL},

  {PERIPH_GPIO_A, GPIOA},
  {PERIPH_GPIO_B, GPIOB},
  {PERIPH_GPIO_C, GPIOC},
  {PERIPH_GPIO_H, GPIOH},
};

/* IO External interrupt */
io_ext_irq_hw_info_t const io_ext_irq_hw_info[IO_NUM_OF_EXT_IRQ] =
{
  /* IO_EXT_IRQ_1 */
  {
    .io_num   = IO_portPinToNum(IO_PORT_NULL, IO_NULL_PIN),
    .irq_num  = EXTI1_IRQn,
    .priority = PRIORITY_MEDIUM
  },
};


/* DMA */
dma_hw_info_t const dma_hw_info[DMA_NUM_OF_STREAM] =
{
  /* periph       inst          irq_num */

  /* DMA_STREAM_NONE: NULL stream to signify not using dma */
  {0,             NULL,         0},

  {PERIPH_DMA_1,  DMA1_Stream0, DMA1_Stream0_IRQn},
  {PERIPH_DMA_1,  DMA1_Stream6, DMA1_Stream6_IRQn},
  {PERIPH_DMA_2,  DMA2_Stream0, DMA2_Stream0_IRQn},
  {PERIPH_DMA_2,  DMA2_Stream3, DMA2_Stream3_IRQn},
};


/* USART */
usart_hw_info_t const usart_hw_info[USART_NUM_OF_CH] =
{
  /* USART_CH_2 */
  {
    .periph         = PERIPH_USART_2,
    .inst           = USART2,
    .tx_pin         = IO_portPinToNum(IO_PORT_A, 2),
    .rx_pin         = IO_portPinToNum(IO_PORT_A, 3),
    .rts_pin        = IO_NULL_PIN,
    .cts_pin        = IO_NULL_PIN,
    .io_cfg_ext     = {GPIO_AF7_USART2},
    .irq_num        = USART2_IRQn,
    .irq_priority   = USART_2_PRIORITY,
    .dma_tx_stream  = USART_2_TX_DMA_STREAM,
    .dma_tx_ch      = USART_2_TX_DMA_CH,
    .dma_rx_stream  = USART_2_RX_DMA_STREAM,
    .dma_rx_ch      = USART_2_RX_DMA_CH,
  },
};


/* I2C */
i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH] =
{
  /* I2C_CH_1 */
  {
    .periph         = PERIPH_I2C_1,
    .inst           = I2C1,
    .sda_pin        = IO_portPinToNum(IO_PORT_B, 7),
    .scl_pin        = IO_portPinToNum(IO_PORT_B, 6),
    .io_cfg_ext     = {GPIO_AF4_I2C1},
    .event_irq_num  = I2C1_EV_IRQn,
    .error_irq_num  = I2C1_ER_IRQn,
    .irq_priority   = I2C_1_PRIORITY,
    .dma_tx_stream  = I2C_1_TX_DMA_STREAM,
    .dma_tx_ch      = I2C_1_TX_DMA_CH,
    .dma_rx_stream  = I2C_1_RX_DMA_STREAM,
    .dma_rx_ch      = I2C_1_RX_DMA_CH,
  },
};


/* SPI */
spi_hw_info_t const spi_hw_info[SPI_NUM_OF_CH] =
{
  /* SPI_CH_1 */
  {
    .periph         = PERIPH_SPI_1,
    .inst           = SPI1,
    .mosi_pin       = IO_portPinToNum(IO_PORT_A, 7),
    .miso_pin       = IO_portPinToNum(IO_PORT_A, 6),
    .sck_pin        = IO_portPinToNum(IO_PORT_A, 5),
    .io_cfg_ext     = {GPIO_AF5_SPI1},
    .irq_num        = SPI1_IRQn,
    .irq_priority   = SPI_1_PRIORITY,
    .dma_tx_stream  = SPI_1_TX_DMA_STREAM,
    .dma_tx_ch      = SPI_1_TX_DMA_CH,
    .dma_rx_stream  = SPI_1_RX_DMA_STREAM,
    .dma_rx_ch      = SPI_1_RX_DMA_CH,
  },
};
