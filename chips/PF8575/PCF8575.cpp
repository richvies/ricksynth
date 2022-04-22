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


#include "PCF8575.hpp"


PCF8575::PCF8575(I2C_ch_e ch, uint8_t addr)
{
  _i2c_ch         = ch;
  _xfer_info.addr = addr;
  _xfer_info.cb   = NULL;
  _copy           = 0;
}

PCF8575::init(void)
{
  I2C_cfg_t cfg;
  cfg.master_mode   = I2C_MASTER_MODE;
  cfg.clk_speed_hz  = 100000;
  cfg.address_mode  = I2C_ADDRESS_MODE_7_BIT;
  cfg.own_address   = 0;
  cfg.stretch_mode  = I2C_STRETCH_MODE_DISABLE;

  write16(0);

  return I2C_init(_i2c_ch, &cfg);
}


uint16_t PCF8575::read16()
{
  uint16_t read;

  _xfer_info.length = 2;
  _xfer_info.data   = (uint8_t*)&read;

  if (true == I2C_read(_i2c_ch, &_xfer_info))
  {
    while (true == I2C_isBusy(_i2c_ch));
  }
  else
  {
    read = 0;
  }

  return read;
}

void PCF8575::write16(uint16_t value)
{
  _xfer_info.length = 2;
  _xfer_info.data   = (uint8_t*)&value;

  if (true == I2C_write(_i2c_ch, &_xfer_info))
  {
    _copy = value;
  }
}

void PCF8575::toggle16(uint16_t mask)
{
  uint16_t tmp = _copy ^ mask;
  write16(tmp);
}


bool PCF8575::read(uint8_t pin)
{
  return ((read16() & (1 << pin)) > 0);
}

void PCF8575::write(uint8_t pin, bool high)
{
  uint16_t tmp = _copy;

  if (high)
  {
    tmp |= (1 << pin);
  }
  else
  {
    tmp &= ~(1 << pin);
  }

  write16(tmp);
}

void PCF8575::toggle(uint8_t pin)
{
  toggle16(1 << pin);
}


void PCF8575::shiftRight(uint8_t n)
{
  uint16_t tmp = _copy >> n;
  write16(tmp);
}

void PCF8575::shiftLeft(uint8_t n)
{
  uint16_t tmp = _copy << n;
  write16(tmp);
}

void PCF8575::rotateRight(uint8_t n)
{
  n &= 15;
  uint16_t tmp = (_copy >> n) | (_copy << (16 - n));
  write16(tmp);
}

void PCF8575::rotateLeft(uint8_t n)
{
  n &= 15;
  rotateRight(16 - n);
}
