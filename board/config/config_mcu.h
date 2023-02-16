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


#define SPI_1_ENABLED
#define I2C_1_ENABLED
#define USART_2_ENABLED
#define IO_EXT_IRQ_1_ENABLED
#define ADC_1_ENABLED


#ifdef  SPI_1_ENABLED
  #define SPI_1_IO_CFG_EXT      {GPIO_AF5_SPI1}
  #define SPI_1_MOSI_PIN        IO_portPinToNum(IO_PORT_A, 7)
  #define SPI_1_MISO_PIN        IO_portPinToNum(IO_PORT_A, 6)
  #define SPI_1_SCK_PIN         IO_portPinToNum(IO_PORT_A, 5)
  #define SPI_1_PRIORITY        PRIORITY_MEDIUM
  #define SPI_1_RX_DMA_STREAM   DMA_STREAM_NONE
  #define SPI_1_RX_DMA_CH       DMA_CH_3
  #define SPI_1_TX_DMA_STREAM   DMA_STREAM_NONE
  #define SPI_1_TX_DMA_CH       DMA_CH_3
#endif

#ifdef  I2C_1_ENABLED
  #define I2C_1_SDA_PIN         IO_portPinToNum(IO_PORT_B, 7)
  #define I2C_1_SCL_PIN         IO_portPinToNum(IO_PORT_B, 6)
  #define I2C_1_IO_CFG_EXT      {GPIO_AF4_I2C1}
  #define I2C_1_PRIORITY        PRIORITY_MEDIUM
  #define I2C_1_RX_DMA_STREAM   DMA_1_STREAM_0
  #define I2C_1_RX_DMA_CH       DMA_CH_1
  #define I2C_1_TX_DMA_STREAM   DMA_1_STREAM_6
  #define I2C_1_TX_DMA_CH       DMA_CH_1
#endif

#ifdef  USART_2_ENABLED
  #define USART_2_TX_PIN        IO_portPinToNum(IO_PORT_A, 2)
  #define USART_2_RX_PIN        IO_portPinToNum(IO_PORT_A, 3)
  #define USART_2_RTS_PIN       IO_NULL_PIN
  #define USART_2_CTS_PIN       IO_NULL_PIN
  #define USART_2_IO_CFG_EXT    {GPIO_AF7_USART2}
  #define USART_2_PRIORITY      PRIORITY_MEDIUM
  #define USART_2_RX_DMA_STREAM DMA_STREAM_NONE
  #define USART_2_RX_DMA_CH     0
  #define USART_2_TX_DMA_STREAM DMA_STREAM_NONE
  #define USART_2_TX_DMA_CH     0
#endif

#ifdef  IO_EXT_IRQ_1_ENABLED
  #define IO_EXT_IRQ_1_PIN      IO_portPinToNum(IO_PORT_NULL, IO_NULL_PIN)
  #define IO_EXT_IRQ_1_PRIORITY PRIORITY_MEDIUM
#endif

#ifdef  ADC_1_ENABLED
  #define ADC_1_DMA_STREAM      DMA_1_STREAM_0
#endif


#ifdef __cplusplus
}
#endif


#endif
