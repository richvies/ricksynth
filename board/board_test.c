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

#include "board.h"
#include "config_board.h"
#include "chips.h"

#include "tim.h"


bool BTST_test_onboard_led(void)
{
  IO_cfg_t cfg =
  {
    .mode = IO_MODE_GPIO_OUT_PP,
    .speed = IO_SPEED_SLOW,
    .pullup = IO_PULL_NONE,
    .extend = NULL,
  };

  IO_configure(BUILTIN_LED_PIN, &cfg);

  while (1)
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      IO_set(BUILTIN_LED_PIN);
      TIM_delayMs((i+1)*100);
      IO_clear(BUILTIN_LED_PIN);
      TIM_delayMs((i+1)*100);
    }
  }
}

bool BTST_W25Q(void)
{
  bool ret = false;
  char *data = "hello there how are you?";
  char read[64];

  if (W25Q_init())
    if (W25Q_eraseSector(0))
      if (W25Q_programSector(0, 0, (uint8_t*)data, strlen(data) + 1))
        if (W25Q_readSector(0, 0, (uint8_t*)read, sizeof(read)))
          if (0 == strncmp(data, read, strlen(data)))
          {
            ret = true;
          }

  return ret;
}

bool BTST_test_lights(void)
{
  uint16_t i;

  PCF8575_init(0, PCF8575_0_CH, PCF8575_0_ADDR);

  for (i = 0; i < 10; i++)
  {
    IO_toggle(BUILTIN_LED_PIN);
    PCF8575_rotateRight(0, 1, NULL);
    TIM_delayMs(100);
  }

  return true;
}
