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


#include "config_board.h"


#ifdef PCF8575_NUM_OF


#include "pcf8575.h"


typedef struct
{
  I2C_ch_e ch;
  uint8_t  addr;
  PCF8575_cb_t cb;
  bool lock;
  uint8_t reg[2];
} pcf8575_t;


static pcf8575_t pcf8575[PCF8575_NUM_OF] = {0};

static uint16_t getReg(pcf8575_t *p);
static void setReg(pcf8575_t *p, uint16_t val);
static void i2cCb(bool error, void *ctx);


bool PCF8575_init(uint8_t idx, I2C_ch_e ch, uint8_t addr)
{
  bool ret = false;
  pcf8575_t *p = &pcf8575[idx];

  I2C_cfg_t cfg;
  cfg.master_mode   = I2C_MASTER_MODE;
  cfg.clk_speed_hz  = 100000;
  cfg.address_mode  = I2C_ADDRESS_MODE_7_BIT;
  cfg.own_address   = 0;
  cfg.stretch_mode  = I2C_STRETCH_MODE_DISABLE;

  p->ch = ch;
  p->addr = addr;
  p->cb = NULL;
  p->lock = false;

  if (true == I2C_init(ch, &cfg))
  {
    while (false == PCF8575_write16(idx, 1, NULL));
    ret = true;
  }

  return ret;
}

bool PCF8575_write16(uint8_t idx, uint16_t value, PCF8575_cb_t cb)
{
  bool ret = false;
  pcf8575_t *p = &pcf8575[idx];
  uint16_t tmp = getReg(p);

  if (false == p->lock)
  {
    p->lock = true;
    p->cb = cb;
    setReg(p, value);

    if (true == I2C_write(p->ch, p->addr, p->reg, 2, i2cCb, p)) {
      ret = true; }
    else
    {
      setReg(p, tmp);
      p->lock = false;
    }
  }

  return ret;
}

bool PCF8575_write(uint8_t idx, uint8_t pin, bool high, PCF8575_cb_t cb)
{
  pcf8575_t *p = &pcf8575[idx];
  uint16_t tmp = getReg(p);

  if (high) {
    tmp |= (1 << pin); }
  else {
    tmp &= ~(1 << pin); }

  return PCF8575_write16(idx, tmp, cb);
}

bool PCF8575_toggle16(uint8_t idx, uint16_t mask, PCF8575_cb_t cb)
{
  pcf8575_t *p = &pcf8575[idx];

  return PCF8575_write16(idx, getReg(p) ^ mask, cb);
}

bool PCF8575_toggle(uint8_t idx, uint8_t pin, PCF8575_cb_t cb)
{
  return PCF8575_toggle16(idx, 1 << pin, cb);
}


bool PCF8575_shiftRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  pcf8575_t *p = &pcf8575[idx];
  uint16_t tmp = getReg(p) >> n;
  return PCF8575_write16(idx, tmp, cb);
}

bool PCF8575_shiftLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  pcf8575_t *p = &pcf8575[idx];
  uint16_t tmp = getReg(p) << n;
  return PCF8575_write16(idx, tmp, cb);
}

bool PCF8575_rotateRight(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  n &= 0xF;
  pcf8575_t *p = &pcf8575[idx];
  uint16_t tmp = (getReg(p) >> n) | (getReg(p) << (16 - n));
  return PCF8575_write16(idx, tmp, cb);
}

bool PCF8575_rotateLeft(uint8_t idx, uint8_t n, PCF8575_cb_t cb)
{
  n &= 15;
  return PCF8575_rotateRight(idx, 16 - n, cb);
}


static uint16_t getReg(pcf8575_t *p)
{
  return ((uint16_t)p->reg[1] << 8) + p->reg[0];
}

static void setReg(pcf8575_t *p, uint16_t val)
{
  p->reg[0] = (uint8_t)val;
  p->reg[1] = (uint8_t)(val >> 8);
}

static void i2cCb(bool error, void *ctx)
{
  pcf8575_t *p = (pcf8575_t*)ctx;

  if (p->cb)
  {
    p->cb(error);
  }
  p->lock = false;
}


#endif
