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


#ifndef __MERR_H
#define __MERR_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"


typedef enum
{
  MERROR_SPI_INIT,
  MERROR_SPI_BUSY_TIMEOUT,
  MERROR_SPI_XFER_START,
  MERROR_SPI_XFER_Q_OVERFLOW,
  MERROR_SPI_XFER_ERROR,

  MERROR_I2C_INIT,
  MERROR_I2C_BUSY_TIMEOUT,
  MERROR_I2C_XFER_START,
  MERROR_I2C_XFER_Q_OVERFLOW,
  MERROR_I2C_XFER_ERROR,

  MERROR_STG_MOUNT_FAIL,
  MERROR_STG_UNMOUNT_FAIL,
  MERROR_STG_FORMAT_FAIL,
  MERROR_STG_NEW_FILE,
  MERROR_STG_OPEN_FILE,
  MERROR_STG_WRITE_FILE,
  MERROR_STG_APPEND_FILE,
  MERROR_STG_READ_FILE,
  MERROR_STG_CLOSE_FILE,
} merror_e;


extern void  MERR_error     (merror_e err, uint32_t arg);
extern void  MERR_errorExt  (merror_e err, uint32_t* arg, uint8_t n_arg);


#ifdef __cplusplus
}
#endif


#endif
