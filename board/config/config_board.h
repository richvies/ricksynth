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


#ifndef __CONFIG_BOARD_H
#define __CONFIG_BOARD_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"

#include "io.h"
#include "tim.h"


#define W25_NUM_OF        1
#define W25_SPI_CH        SPI_CH_1
#define W25_CS_PIN        IO_portPinToNum(IO_PORT_B, 8)

#define STG_0
#define STG_0_READ_SIZE     W25Q_READ_SIZE
#define STG_0_PROG_SIZE     W25Q_PROG_SIZE
#define STG_0_SECTOR_SIZE   W25Q_SECTOR_SIZE
#define STG_0_SECTOR_COUNT  W25Q_SECTOR_COUNT

#define PCF8575_NUM_OF    1
#define PCF8575_0_CH      I2C_CH_1
#define PCF8575_0_ADDR    0

#define BUILTIN_LED_PIN   IO_portPinToNum(IO_PORT_C, 13)


#ifdef __cplusplus
}
#endif


#endif
