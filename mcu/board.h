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


#ifndef __BOARD_H
#define __BOARD_H


#include "stdint.h"
#include "stdbool.h"


/* IO Pins */
typedef uint16_t IO_num_e;

/* IO External interrupt */
typedef enum
{
  IO_EXT_IRQ_FIRST = 0,
  IO_EXT_IRQ_1 = IO_EXT_IRQ_FIRST,
  IO_NUM_OF_EXT_IRQ,
} IO_ext_irq_e;


/* I2C */
typedef enum
{
  I2C_CH_FIRST = 0,
  I2C_CH_1 = I2C_CH_FIRST,
  I2C_NUM_OF_CH,
} I2C_ch_e;


extern const IO_num_e CHIP_spi_nss_pin;


#endif