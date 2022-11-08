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


#include "pcf8575.h"

#include "config_board.h"


typedef struct
{
  I2C_ch_e ch;
  uint8_t  addr;
  PCF8575_cb_t cb;
} pcf8575_t;


static pcf8575_t pcf8575[PCF8575_NUM_OF] = {0};


bool PCF8575_init(uint8_t idx, uint8_t num, I2C_ch_e ch, uint8_t addr)
{
  bool ret = false;

  I2C_cfg_t cfg;
  cfg.master_mode   = I2C_MASTER_MODE;
  cfg.clk_speed_hz  = 100000;
  cfg.address_mode  = I2C_ADDRESS_MODE_7_BIT;
  cfg.own_address   = 0;
  cfg.stretch_mode  = I2C_STRETCH_MODE_DISABLE;

  pcf8575[idx].ch = ch;
  pcf8575[idx].addr = addr;
  pcf8575[idx].cb = NULL;

  if (true == I2C_init(ch, &cfg))
  {
    while (false == I2C_writeReadBlocking(ch, ));
    ret = true;
  }

  return ret;
}


bool PCF8575_read16(uint8_t idx, uint16_t *value, PCF8575_cb_t cb)
{
  I2C_write
  _i2c_xfer_info.length = 2;
  _i2c_xfer_info.data   = (uint8_t*)value;
  _i2c_xfer_info.cb     = i2cReadCb;
  _i2c_xfer_info.ctx    = this;

  _busy = I2C_read(i2c_ch, &_i2c_xfer_info);

  return _busy;
}

bool PCF8575_write16(uint8_t idx, uint16_t value, PCF8575_cb_t cb)
{
  if (true == _busy)
  {
    return false;
  }

  _write = value;
  _xferCb = cb;

  _i2c_xfer_info.length = 2;
  _i2c_xfer_info.data   = (uint8_t*)&_write;
  _i2c_xfer_info.cb     = i2cWriteCb;
  _i2c_xfer_info.ctx    = this;

  _busy = I2C_write(i2c_ch, &_i2c_xfer_info);

  return _busy;
}

bool PCF8575_toggle16(uint8_t idx, uint16_t mask, PCF8575_cb_t cb)
{
  uint16_t tmp = _write ^ mask;
  return write16(tmp, cb);
}


bool PCF8575_read(uint8_t idx, uint8_t pin, uint16_t *value, PCF8575_cb_t cb)
{
  _pin = pin;
  return read16(value, cb);
}

bool PCF8575_write(uint8_t idx, uint8_t pin, bool high, PCF8575_cb_t cb)
{
  uint16_t tmp;

  if (high)
  {
    tmp = _write | (1 << pin);
  }
  else
  {
    tmp = _write & ~(1 << pin);
  }

  return write16(tmp, cb);
}

bool PCF8575_toggle(uint8_t idx, uint8_t pin, PCF8575_cb_t cb)
{
  return toggle16(1 << pin, cb);
}


bool PCF8575_shiftRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  uint16_t tmp = _write >> n;
  return write16(tmp, cb);
}

bool PCF8575_shiftLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  uint16_t tmp = _write << n;
  return write16(tmp, cb);
}

bool PCF8575_rotateRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  n &= 0xF;
  uint16_t tmp = (_write >> n) | (_write << (16 - n));
  return write16(tmp, cb);
}

bool PCF8575_rotateLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  n &= 15;
  return rotateRight(16 - n, cb);
}


static void i2cWriteCb(bool error, void *ctx)
{
  PCF8575 *t = (PCF8575*)ctx;

  t->_busy = false;
  if (t->_xferCb)
  {
    t->_xferCb(error);
  }
}

static void i2cReadCb(bool error, void *ctx)
{
  PCF8575 *t = (PCF8575*)ctx;
  uint16_t val;

  /* get value of specified pin from entire 16 bit read */
  if ((0xff != t->_pin)
  &&  (false == error))
  {
    val = (t->_i2c_xfer_info.data[0] << 8) | t->_i2c_xfer_info.data[1];
    t->_i2c_xfer_info.data[0] = val & (1 << t->_pin);
    t->_pin = 0xff;
  }

  t->_busy = false;
  if (t->_xferCb)
  {
    t->_xferCb(error);
  }
}
