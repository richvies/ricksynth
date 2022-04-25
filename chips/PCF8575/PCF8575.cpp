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
  _i2c_ch             = ch;
  _i2c_xfer_info.addr = addr;
  _i2c_xfer_info.cb   = NULL;
  _copy               = 0;
  _tmp                = 0;
  _readCb             = NULL;
  _pin                = 0xff;
}

bool PCF8575::init(void)
{
  bool ret = false;

  I2C_cfg_t cfg;
  cfg.master_mode   = I2C_MASTER_MODE;
  cfg.clk_speed_hz  = 100000;
  cfg.address_mode  = I2C_ADDRESS_MODE_7_BIT;
  cfg.own_address   = 0;
  cfg.stretch_mode  = I2C_STRETCH_MODE_DISABLE;

  if (true == I2C_init(_i2c_ch, &cfg))
  {
    while (false == write16(0));
    ret = true;
  }

  return ret;
}


bool PCF8575::read16(uint16_t *value, PCF8575_read_cb cb)
{
  _i2c_xfer_info.length = 2;
  _i2c_xfer_info.data   = (uint8_t*)value;
  _i2c_xfer_info.cb     = i2cReadCb;
  _i2c_xfer_info.ctx    = this;

  _readCb = cb;

  return I2C_read(_i2c_ch, &_i2c_xfer_info);
}

bool PCF8575::write16(uint16_t *value)
{
  _i2c_xfer_info.length = 2;
  _i2c_xfer_info.data   = (uint8_t*)value;
  _i2c_xfer_info.cb     = i2cWriteCb;
  _i2c_xfer_info.ctx    = this;

  return I2C_write(_i2c_ch, &_i2c_xfer_info);
}

bool PCF8575::toggle16(uint16_t mask)
{
  _tmp = _copy ^ mask;
  return write16(&_tmp);
}


bool PCF8575::read(uint8_t pin, uint16_t *value, PCF8575_read_cb cb)
{
  _pin = pin;
  return read16(value, cb);
}

bool PCF8575::write(uint8_t pin, bool high)
{
  _tmp = _copy;

  if (high)
  {
    _tmp |= (1 << pin);
  }
  else
  {
    _tmp &= ~(1 << pin);
  }

  return write16(&_tmp);
}

bool PCF8575::toggle(uint8_t pin)
{
  return toggle16(1 << pin);
}


bool PCF8575::shiftRight(uint8_t n)
{
  _tmp = _copy >> n;
  return write16(&_tmp);
}

bool PCF8575::shiftLeft(uint8_t n)
{
  _tmp = _copy << n;
  return write16(&_tmp);
}

bool PCF8575::rotateRight(uint8_t n)
{
  n &= 15;
  _tmp = (_copy >> n) | (_copy << (16 - n));
  return write16(&_tmp);
}

bool PCF8575::rotateLeft(uint8_t n)
{
  n &= 15;
  return rotateRight(16 - n);
}


void PCF8575::i2cWriteCb(bool error, void *ctx)
{
  PCF8575 *t = (PCF8575*)ctx;

  if (false == error)
  {
    t->_copy = *((uint16_t*)t->_i2c_xfer_info.data);
  }
}

void PCF8575::i2cReadCb(bool error, void *ctx)
{
  uint16_t value;
  PCF8575 *t = (PCF8575*)ctx;

  if ((0xff != t->_pin)
  &&  (false == error))
  {
    /* get value of specified pin from entire 16 bit read */
    value   = *((uint16_t*)t->_i2c_xfer_info.data);
    t->_i2c_xfer_info.data[0] = value & (1 << t->_pin);

    t->_pin = 0xff;
  }

  t->_readCb(error);
}