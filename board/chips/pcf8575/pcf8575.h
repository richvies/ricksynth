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


#ifndef _PCF_8575_H
#define _PCF_8575_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "i2c.h"


typedef void (*PCF8575_cb_t)(bool error);


extern bool PCF8575_init(uint8_t idx, uint8_t num, I2C_ch_e ch, uint8_t addr);

extern bool PCF8575_read16(uint8_t idx, uint16_t *value, PCF8575_cb_t cb);
extern bool PCF8575_write16(uint8_t idx, uint16_t value, PCF8575_cb_t cb);
extern bool PCF8575_toggle16(uint8_t idx, uint16_t mask, PCF8575_cb_t cb);

extern bool PCF8575_read(uint8_t idx, uint8_t pin, uint16_t *value, PCF8575_cb_t cb);
extern bool PCF8575_write(uint8_t idx, uint8_t pin, bool high, PCF8575_cb_t cb);
extern bool PCF8575_toggle(uint8_t idx, uint8_t pin, PCF8575_cb_t cb);

extern bool PCF8575_shiftRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb);
extern bool PCF8575_shiftLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb);
extern bool PCF8575_rotateRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb);
extern bool PCF8575_rotateLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb);


#ifdef __cplusplus
}
#endif


#endif
