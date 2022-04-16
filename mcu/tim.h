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


#ifndef __TIM_H
#define __TIM_H


#include "board.h"


typedef void (*TIM_cb)(void);


extern bool     TIM_init(TIM_ch_e ch, uint32_t freq_hz);
extern bool     TIM_start(TIM_ch_e ch, uint32_t period, TIM_cb callback_fn);
extern bool     TIM_stop(TIM_ch_e ch);
extern bool     TIM_updateTimerFrequency(TIM_ch_e ch, uint32_t freq_hz);
extern void     TIM_delayUs(uint32_t delay);
extern void     TIM_delayMs(uint32_t delay);
extern uint32_t TIM_millis(void);


#endif
