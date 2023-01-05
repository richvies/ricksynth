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


#ifndef __SPI_H
#define __SPI_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"

#include "io.h"


#define SPI_NO_CS_PIN   IO_NULL_PIN


typedef enum
{
  SPI_MASTER_MODE,
  /// slave mode not implemented yet
  SPI_SLAVE_MODE,
  SPI_NUM_OF_MASTER_MODES,
} SPI_master_mode_e;

typedef enum
{
  SPI_CLK_POLARITY_IDLE_LOW,
  SPI_CLK_POLARITY_IDLE_HIGH,
  SPI_NUM_OF_CLK_POLARITY,
} SPI_clk_polarity_e;

typedef enum
{
  SPI_CLK_PHASE_SAMPLE_FIRST_EDGE,
  SPI_CLK_PHASE_SAMPLE_SECOND_EDGE,
  SPI_NUM_OF_CLK_PHASES,
} SPI_clk_phase_e;

typedef enum
{
  SPI_BIT_ORDER_MSB_FIRST,
  SPI_BIT_ORDER_LSB_FIRST,
  SPI_NUM_OF_BIT_ORDER,
} SPI_bit_order_e;


typedef struct
{
  IO_num_e            cs_pin;
  SPI_master_mode_e   master_mode;
  SPI_clk_polarity_e  clk_polarity;
  SPI_clk_phase_e     clk_phase;
  SPI_bit_order_e     bit_order;
  uint32_t            clk_speed_hz;
} SPI_cfg_t;

typedef void (*SPI_xfer_cb)(bool done, bool error, void *ctx);


/**
 * @brief configures peripheral, clocks, io, enables interrups (& DMA if used)
 *
 * @return true if channel is started or was previously started
 */
extern bool SPI_init      (SPI_ch_e ch, SPI_cfg_t *cfg);

/**
 * @brief stops peripheral, clocks, io, disables interrupts (& DMA if used)
 *
 * @return true if channel stoped or previously stopped
 */
extern bool SPI_deInit    (SPI_ch_e ch);

/**
 * @brief checks if peripheral is ready for a new transaction
 */
extern bool SPI_isBusy    (SPI_ch_e ch);

/**
 * @brief Call this function periodically if using spi bus
 * @attention Do NOT call from an interrupt
 */
extern void SPI_task      (void);

/**
 * @brief transfer data to/ from a device on spi bus using interrupts or dma
 * @attention Do NOT call from an interrupt
 *
 * @return true if added to queue succesfully
 */
extern bool SPI_write       (SPI_ch_e    ch,
                             IO_num_e    cs_pin,
                             uint8_t*    tx_data,
                             uint16_t    length,
                             SPI_xfer_cb cb,
                             void*       ctx);

extern bool SPI_read        (SPI_ch_e    ch,
                             IO_num_e    cs_pin,
                             uint8_t*    rx_data,
                             uint16_t    length,
                             SPI_xfer_cb cb,
                             void*       ctx);

extern bool SPI_writeRead   (SPI_ch_e    ch,
                             IO_num_e    cs_pin,
                             uint8_t*    tx_data,
                             uint8_t*    rx_data,
                             uint16_t    length,
                             SPI_xfer_cb cb,
                             void*       ctx);

/**
 * @brief transfer data to/ from a device on spi bus and wait to complete
 * @attention Do NOT call from an interrupt
 *
 * @return true if completed succesfully
 */
extern bool SPI_writeBlocking (SPI_ch_e    ch,
                               IO_num_e    cs_pin,
                               uint8_t*    tx_data,
                               uint16_t    length);

extern bool SPI_readBlocking  (SPI_ch_e    ch,
                               IO_num_e    cs_pin,
                               uint8_t*    rx_data,
                               uint16_t    length);

extern bool SPI_writeReadBlocking (SPI_ch_e    ch,
                                   IO_num_e    cs_pin,
                                   uint8_t*    tx_data,
                                   uint8_t*    rx_data,
                                   uint16_t    length);


#ifdef __cplusplus
}
#endif


#endif
