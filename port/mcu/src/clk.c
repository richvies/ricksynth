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


#include "clk.h"


void CLK_periphEnable(periph_e periph)
{
  PRINTF_INFO("enable %u", periph);

  switch (periph)
  {
    case PERIPH_GPIO_A:
      __HAL_RCC_GPIOA_CLK_ENABLE();
      break;

    case PERIPH_GPIO_B:
      __HAL_RCC_GPIOB_CLK_ENABLE();
      break;

    case PERIPH_GPIO_C:
      __HAL_RCC_GPIOC_CLK_ENABLE();
      break;

    case PERIPH_I2C_1:
      __HAL_RCC_I2C1_CLK_ENABLE();
      break;

    default:
      PRINTF_WARN("%u unknown", periph);
      break;
  }
}

void CLK_periphReset(periph_e periph)
{
  PRINTF_INFO("reset %u", periph);

  switch (periph)
  {
    case PERIPH_GPIO_A:
      __HAL_RCC_GPIOA_CLK_ENABLE();
      break;

    case PERIPH_GPIO_B:
      __HAL_RCC_GPIOB_CLK_ENABLE();
      break;

    case PERIPH_GPIO_C:
      __HAL_RCC_GPIOC_CLK_ENABLE();
      break;

    case PERIPH_I2C_1:
      __HAL_RCC_I2C3_FORCE_RESET();
      __HAL_RCC_I2C3_RELEASE_RESET();
      break;

    default:
      PRINTF_WARN("%u reset", periph);
      break;
  }
}

void CLK_periphResetAll(void)
{
  HAL_DeInit();
}
