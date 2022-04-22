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


#include "board.h"


typedef enum
{
  I2C_MASTER_MODE,
  I2C_SLAVE_MODE,
  I2C_NUM_OF_MASTER_MODES,
} I2C_master_mode_e;

typedef enum
{
  I2C_ADDRESS_MODE_7_BIT,
  I2C_ADDRESS_MODE_10_BIT,
  I2C_NUM_OF_ADDRESS_MODES,
} I2C_address_mode_e;

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

typedef void (*I2C_xfer_cb)(bool error);

typedef struct
{
  uint16_t addr;
  uint8_t *data;
  uint16_t length;
  I2C_xfer_cb cb;
} I2C_xfer_info_t;


extern bool I2C_init    (I2C_ch_e ch, I2C_cfg_t *cfg);
extern bool I2C_deInit  (I2C_ch_e ch);
extern bool I2C_isBusy  (I2C_ch_e ch);
extern bool I2C_write   (I2C_ch_e ch, I2C_xfer_info_t *info);
extern bool I2C_read    (I2C_ch_e ch, I2C_xfer_info_t *info);


#ifdef __cplusplus
}
#endif


#endif
