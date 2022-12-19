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


#ifndef __IRQ_H
#define __IRQ_H


#include "mcu.h"


typedef int16_t IRQ_num_e;


typedef enum
{
  PRIORITY_VERY_HIGH,
  PRIORITY_HIGH,
  PRIORITY_MEDIUM,
  PRIORITY_LOW,
  PRIORITY_VERY_LOW,
  PRIORITY_NUM_OF,
} IRQ_priority_e;


extern void irq_config(IRQ_num_e irq, IRQ_priority_e priority);
extern void irq_enable(IRQ_num_e irq);
extern void irq_disable(IRQ_num_e irq);
extern void irq_disableAll(void);

extern IRQ_num_e    irq_get_current(void);
extern void         irq_set_context(IRQ_num_e irq, void *context);
extern void*        irq_get_context(IRQ_num_e irq);


#endif
