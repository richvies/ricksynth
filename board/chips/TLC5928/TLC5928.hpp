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


#ifndef _TLC5928_H
#define _TLC5928_H


#include "io.h"
#include "spi.h"


using TLC5928_xfer_cb = void(*)(bool error);


class TLC5928
{
  public:
    TLC5928(SPI_ch_e ch, IO_num_e cs_pin);

    bool init(void);

    uint16_t getCopy16(void);
    bool write16(uint16_t value, TLC5928_xfer_cb cb);
    bool toggle16(uint16_t mask, TLC5928_xfer_cb cb);

    bool getCopy(uint8_t pin);
    bool write(uint8_t pin, bool high, TLC5928_xfer_cb cb);
    bool toggle(uint8_t pin, TLC5928_xfer_cb cb);

    bool shiftRight(uint8_t n, TLC5928_xfer_cb cb);
    bool shiftLeft(uint8_t n, TLC5928_xfer_cb cb);
    bool rotateRight(uint8_t n, TLC5928_xfer_cb cb);
    bool rotateLeft(uint8_t n, TLC5928_xfer_cb cb);

  private:
    SPI_ch_e        _spi_ch;
    SPI_xfer_info_t _spi_xfer_info;
    static void spiWriteCb(bool error, bool done, void *ctx);

    TLC5928_xfer_cb _xferCb;
    uint16_t  _copy, _tmp;
    bool _busy;
};


#endif
