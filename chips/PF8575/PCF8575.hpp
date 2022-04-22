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


class PCF8575
{
  public:
    void PCF8575(I2C_ch_e ch, uint8_t addr);

    bool      init(void);

    void      write16(uint16_t value);
    uint16_t  read16();
    void      toggle16(uint16_t mask);

    void      write(uint8_t pin, bool high);
    bool      read(uint8_t pin);
    void      toggle(uint8_t pin);

    void shiftRight(uint8_t n = 1);
    void shiftLeft(uint8_t n = 1);
    void rotateRight(uint8_t n = 1);
    void rotateLeft(uint8_t n = 1);

  private:
    I2C_ch_e  _i2c_ch;
    I2C_xfer_info_t _xfer_info;
    uint16_t  _copy;
};

#endif
