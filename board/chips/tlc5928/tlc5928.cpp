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


#include "tlc5928.hpp"

#include "stddef.h"


TLC5928::TLC5928(SPI_ch_e ch, IO_num_e cs_pin)
{
  _spi_ch                 = ch;
  _spi_xfer_info.cs_pin   = cs_pin;
  _spi_xfer_info.cb       = NULL;
  _copy                   = 0;
  _tmp                    = 0;
  _xferCb                 = NULL;
}

bool TLC5928::init(void)
{
  bool ret = false;

  IO_cfg_t io_cfg;
  io_cfg.mode     = IO_MODE_GPIO;
  io_cfg.dir      = IO_DIR_OUT_PP;
  io_cfg.pullup   = IO_PULL_NONE;
  io_cfg.speed    = IO_SPEED_FAST;
  IO_configure(_spi_xfer_info.cs_pin, &io_cfg);
  IO_set(_spi_xfer_info.cs_pin);

  SPI_cfg_t cfg;
  cfg.master_mode   = SPI_MASTER_MODE;
  cfg.clk_speed_hz  = 1000000;
  cfg.bit_order     = SPI_BIT_ORDER_MSB_FIRST;
  cfg.clk_polarity  = SPI_CLK_POLARITY_IDLE_LOW;
  cfg.clk_phase     = SPI_CLK_PHASE_SAMPLE_FIRST_EDGE;

  _busy = false;

  if (true == SPI_init(_spi_ch, &cfg))
  {
    while (false == write16(9, NULL));
    ret = true;
  }

  return ret;
}


uint16_t TLC5928::getCopy16(void)
{
  return _copy;
}

bool TLC5928::write16(uint16_t value, TLC5928_xfer_cb cb)
{
  bool ret = false;

  if (false == _busy)
  {
    _busy = true;
    _tmp = value;
    _xferCb = cb;

    _spi_xfer_info.length   = 2;
    _spi_xfer_info.tx_data  = (uint8_t*)&_tmp;
    _spi_xfer_info.cb       = spiWriteCb;
    _spi_xfer_info.ctx      = this;

    ret = SPI_write(_spi_ch, &_spi_xfer_info);
    if (false == ret)
    {
      _busy = false;
    }
  }

  return ret;
}

bool TLC5928::toggle16(uint16_t mask, TLC5928_xfer_cb cb)
{
  uint16_t v = _copy ^ mask;
  return write16(v, cb);
}


bool TLC5928::getCopy(uint8_t pin)
{
  return _copy & (1 << pin);
}

bool TLC5928::write(uint8_t pin, bool high, TLC5928_xfer_cb cb)
{
  uint16_t v = _copy;

  if (high)
  {
    _tmp |= (1 << pin);
  }
  else
  {
    _tmp &= ~(1 << pin);
  }

  return write16(v, cb);
}

bool TLC5928::toggle(uint8_t pin, TLC5928_xfer_cb cb)
{
  return toggle16(1 << pin, cb);
}


bool TLC5928::shiftRight(uint8_t n, TLC5928_xfer_cb cb)
{
  uint16_t v = _copy >> n;
  return write16(v, cb);
}

bool TLC5928::shiftLeft(uint8_t n, TLC5928_xfer_cb cb)
{
  uint16_t v = _copy << n;
  return write16(v, cb);
}

bool TLC5928::rotateRight(uint8_t n, TLC5928_xfer_cb cb)
{
  n &= 15;
  uint16_t v = (_copy >> n) | (_copy << (16 - n));
  return write16(v, cb);
}

bool TLC5928::rotateLeft(uint8_t n, TLC5928_xfer_cb cb)
{
  n &= 15;
  return rotateRight(16 - n, cb);
}


void TLC5928::spiWriteCb(bool error, bool done, void *ctx)
{
  TLC5928 *t = (TLC5928*)ctx;

  if (done)
  {
    if (false == error)
    {
      t->_copy = *((uint16_t*)t->_spi_xfer_info.tx_data);
    }
    t->_busy = false;
    if (t->_xferCb)
    {
      t->_xferCb(error);
    }
  }
}
