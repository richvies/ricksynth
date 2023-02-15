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


#define SPI_1_HW_INFO \
{ \
  .periph         = PERIPH_SPI_1, \
  .inst           = SPI1, \
  .mosi_pin       = SPI_1_MOSI_PIN, \
  .miso_pin       = SPI_1_MISO_PIN, \
  .sck_pin        = SPI_1_SCK_PIN, \
  .io_cfg_ext     = SPI_1_IO_CFG_EXT, \
  .irq_num        = SPI1_IRQn, \
  .irq_priority   = SPI_1_PRIORITY, \
  .dma_tx_stream  = SPI_1_TX_DMA_STREAM, \
  .dma_tx_ch      = SPI_1_TX_DMA_CH, \
  .dma_rx_stream  = SPI_1_RX_DMA_STREAM, \
  .dma_rx_ch      = SPI_1_RX_DMA_CH, \
}

#define I2C_1_HW_INFO \
{ \
  .periph         = PERIPH_I2C_1, \
  .inst           = I2C1, \
  .sda_pin        = I2C_1_SDA_PIN, \
  .scl_pin        = I2C_1_SCL_PIN, \
  .io_cfg_ext     = I2C_1_IO_CFG_EXT, \
  .event_irq_num  = I2C1_EV_IRQn, \
  .error_irq_num  = I2C1_ER_IRQn, \
  .irq_priority   = I2C_1_PRIORITY, \
  .dma_tx_stream  = I2C_1_TX_DMA_STREAM, \
  .dma_tx_ch      = I2C_1_TX_DMA_CH, \
  .dma_rx_stream  = I2C_1_RX_DMA_STREAM, \
  .dma_rx_ch      = I2C_1_RX_DMA_CH, \
}

#define USART_2_HW_INFO \
{ \
  .periph         = PERIPH_USART_2, \
  .inst           = USART2, \
  .tx_pin         = USART_2_TX_PIN, \
  .rx_pin         = USART_2_RX_PIN, \
  .rts_pin        = USART_2_RTS_PIN, \
  .cts_pin        = USART_2_CTS_PIN, \
  .io_cfg_ext     = USART_2_IO_CFG_EXT, \
  .irq_num        = USART2_IRQn, \
  .irq_priority   = USART_2_PRIORITY, \
  .dma_tx_stream  = USART_2_TX_DMA_STREAM, \
  .dma_tx_ch      = USART_2_TX_DMA_CH, \
  .dma_rx_stream  = USART_2_RX_DMA_STREAM, \
  .dma_rx_ch      = USART_2_RX_DMA_CH, \
  }


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
#ifdef USART_1
  USART_1_HW_INFO,
#endif

#ifdef USART_2
  USART_2_HW_INFO,
#endif
};


/* I2C */
i2c_hw_info_t const i2c_hw_info[I2C_NUM_OF_CH] =
{
#ifdef I2C_1
  I2C_1_HW_INFO,
#endif
};


/* SPI */
spi_hw_info_t const spi_hw_info[SPI_NUM_OF_CH] =
{
#ifdef SPI_1
  SPI_1_HW_INFO,
#endif

#ifdef SPI_2
  SPI_2_HW_INFO,
#endif

#ifdef SPI_3
  SPI_3_HW_INFO,
#endif
};
