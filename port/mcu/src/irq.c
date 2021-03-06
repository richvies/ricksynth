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


#include "irq.h"


static uint8_t const irq_to_nvic[PRIORITY_NUM_OF] =
{
  1,
  4,
  7,
  11,
  15,
};

void IRQ_config(irq_num_e irq, irq_priority_e priority)
{
  HAL_NVIC_SetPriority(irq, 0, irq_to_nvic[priority]);
}

void IRQ_enable(irq_num_e irq)
{
  HAL_NVIC_EnableIRQ(irq);
}

void IRQ_disable(irq_num_e irq)
{
  HAL_NVIC_DisableIRQ(irq);
}

void IRQ_disableAll(void)
{
  irq_num_e irq;

  for (irq = 0; irq < 0x7f; irq++)
  {
    HAL_NVIC_DisableIRQ(irq);
  }
}
