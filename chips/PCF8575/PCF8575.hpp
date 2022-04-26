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


#include "i2c.h"

using PCF8575_read_cb = void(*)(bool error);

class PCF8575
{
  public:
    PCF8575(I2C_ch_e ch, uint8_t addr);

    bool init(void);

    bool read16(uint16_t *value, PCF8575_read_cb cb);
    bool write16(uint16_t value);
    bool toggle16(uint16_t mask);

    bool read(uint8_t pin, uint16_t *value, PCF8575_read_cb cb);
    bool write(uint8_t pin, bool high);
    bool toggle(uint8_t pin);

    bool shiftRight(uint8_t n);
    bool shiftLeft(uint8_t n);
    bool rotateRight(uint8_t n);
    bool rotateLeft(uint8_t n);

  private:
    I2C_ch_e        _i2c_ch;
    I2C_xfer_info_t _i2c_xfer_info;
    static void i2cReadCb(bool error, void *ctx);
    static void i2cWriteCb(bool error, void *ctx);

    PCF8575_read_cb _readCb;
    uint16_t  _copy, _tmp;
    uint8_t _pin;
};

#endif
