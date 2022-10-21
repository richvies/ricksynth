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



#include "board_test.h"
#include "io.h"
#include "tim.h"


int main()
{
  // IO_cfg_t cfg;
  // PCF8575 io_exp(I2C_CH_1, 0x40);
  // TLC5928 leds(SPI_CH_1, TLC5928_SPI_NSS_PIN);

  // IO_init();

  // io_exp.init();
  // leds.init();

  // cfg.dir = IO_DIR_OUT_PP;
  // cfg.mode = IO_MODE_GPIO;
  // cfg.pullup = IO_PULL_NONE;
  // cfg.speed = IO_SPEED_FAST;
  // IO_configure(BUILTIN_LED_PIN, &cfg);

  // while(1)
  // {
  //   IO_toggle(BUILTIN_LED_PIN);
  //   io_exp.rotateRight(1, NULL);
  //   leds.rotateRight(1, NULL);
  //   TIM_delayMs(100);
  // }

  return 0;
}
