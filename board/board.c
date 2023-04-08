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


#include "board.h"
#include "config_board.h"

#include "chips.h"

#include "io.h"
#include "mevent.h"
#include "spi.h"


bool BRD_init()
{
  bool ret = true;

  IO_cfg_t io_cfg;

  IO_init();

  io_cfg.mode = IO_MODE_GPIO_OUT_PP;
  io_cfg.pullup = IO_PULL_NONE;
  io_cfg.speed = IO_SPEED_FAST;
  IO_configure(BUILTIN_LED_PIN, &io_cfg);

  return ret;
}

void BRD_task()
{
  mevent_e event;

  if (false == MEVE_isGlobalPending()) {
    return; }

  MEVE_clearGlobal();

  for (event = MEVENT_FIRST; event < MEVENT_NUM_OF; event++)
  {
    if (false == MEVE_isPending(event)) {
      continue; }

    MEVE_clearEvent(event);

    switch (event)
    {
    case MEVENT_I2C:
      I2C_task();
      break;

    default:
      break;
    }
  }
}
