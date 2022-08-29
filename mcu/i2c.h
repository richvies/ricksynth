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


#ifndef __I2C_H
#define __I2C_H


#ifdef __cplusplus
 extern "C" {
#endif


/**
 * @file i2c.h
 * @author Rick Davies (richvies@gmail.com)
 * @brief I2C peripheral interface
 * @version 0.1
 * @date 2022-08-29
 *
 * @copyright Copyright (c) 2022
 *
 */


#include "mcu.h"


typedef enum
{
  I2C_MASTER_MODE,
  /// slave mode not implemented yet
  I2C_SLAVE_MODE,
  I2C_NUM_OF_MASTER_MODES,
} I2C_master_mode_e;

typedef enum
{
  I2C_ADDRESS_MODE_7_BIT,
  I2C_ADDRESS_MODE_10_BIT,
  I2C_NUM_OF_ADDRESS_MODES,
} I2C_address_mode_e;

/// clock stretching
typedef enum
{
  I2C_STRETCH_MODE_ENABLE,
  I2C_STRETCH_MODE_DISABLE,
  I2C_NUM_OF_STRETCH_MODES,
} I2C_stretch_mode_e;


typedef struct
{
  I2C_master_mode_e   master_mode;
  I2C_address_mode_e  address_mode;
  I2C_stretch_mode_e  stretch_mode;
  uint32_t            clk_speed_hz;
  uint16_t            own_address;
} I2C_cfg_t;

typedef void (*I2C_xfer_cb)(bool error, void *ctx);

typedef struct
{
  /// address of device on bus
  uint16_t    addr;
  /// data buffer to write from or read to
  uint8_t*    data;
  /// length in bytes of transaction
  uint16_t    length;
  /// callback to be called when transaction completed. Set to NULL if not needed
  I2C_xfer_cb cb;
  /// will be passed as argument to callback function
  void*       ctx;
} I2C_xfer_info_t;


/**
 * @brief configures peripheral, clocks, io, enables interrups (& DMA if used)
 *
 * @param ch @ref I2C_ch_e
 * @param cfg i2c channel configuration @ref I2C_cfg_t
 * @return true if channel is started or was previously started
 * @return false if channel could not be started
 */
extern bool I2C_init    (I2C_ch_e ch, I2C_cfg_t *cfg);

/**
 * @brief stops peripheral, clocks, io, disables interrupts (& DMA if used)
 *
 * @param ch @ref I2C_ch_e
 * @return true if channel stoped or previously stopped
 * @return false if failed to stop channel
 */
extern bool I2C_deInit  (I2C_ch_e ch);

/**
 * @brief checks if peripheral is ready for a new transaction
 *
 * @param ch @ref I2C_ch_e
 * @return true if transaction is ongoing
 * @return false if channel is free for new transaction
 */
extern bool I2C_isBusy  (I2C_ch_e ch);

/**
 * @brief Call this function periodically if using i2c bus
 * Checks queue for each channel and starts next tranaction
 * if channel is not busy
 * @attention Do NOT call from an interrupt
 *
 */
extern void I2C_task    (void);

/**
 * @brief write data to a device on i2c bus
 * @attention Do NOT call from an interrupt
 *
 * @param ch channel to use @ref I2C_ch_e
 * @param info transaction information @ref I2C_xfer_info_t
 * @return true if write started or added to queue succesfully
 * @return false if not able to start write or add to queue
 */
extern bool I2C_write   (I2C_ch_e ch, I2C_xfer_info_t *info);

/**
 * @brief read data from a device on i2c bus
 * @attention Do NOT call from an interrupt
 *
 * @param ch channel to use @ref I2C_ch_e
 * @param info transaction information @ref I2C_xfer_info_t
 * @return true if read started or added to queue succesfully
 * @return false if not able to start read or add to queue
 */
extern bool I2C_read    (I2C_ch_e ch, I2C_xfer_info_t *info);


#ifdef __cplusplus
}
#endif


#endif
