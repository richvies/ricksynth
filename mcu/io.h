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


#ifndef __IO_H
#define __IO_H


#include "board.h"


typedef enum
{
  IO_DIR_IN,
  IO_DIR_OUT_PP,
  IO_DIR_OUT_OD,
  IO_NUM_OF_DIRECTIONS,
} IO_direction_e;

typedef enum
{
  IO_MODE_GPIO,
  IO_MODE_ANALOG,
  IO_MODE_PERIPH,
  IO_MODE_IT_POS,
  IO_MODE_IT_NEG,
  IO_MODE_IT_BOTH,
  IO_NUM_OF_MODES,
} IO_mode_e;

typedef enum
{
  IO_SPEED_SLOW,
  IO_SPEED_MID,
  IO_SPEED_FAST,
  IO_NUM_OF_SPEEDS,
} IO_speed_e;

typedef enum
{
  IO_PULL_NONE,
  IO_PULL_DOWN,
  IO_PULL_UP,
  IO_PULL_BOTH,
  IO_NUM_OF_PULL_MODES,
} IO_pull_res_e;


typedef struct
{
  IO_direction_e  dir;
  IO_mode_e       mode;
  IO_speed_e      speed;
  IO_pull_res_e   pullup;
  void*           extend;
} IO_cfg_t;


typedef void (*IO_ext_irq_callback_fn)(IO_num_e num);


extern bool IO_init(void);
extern void IO_configure(IO_num_e num, IO_cfg_t *cfg);
extern bool IO_reset(IO_num_e num);

extern void IO_set(IO_num_e num);
extern void IO_clear(IO_num_e num);
extern void IO_toggle(IO_num_e num);
extern bool IO_isHigh(IO_num_e num);

extern bool IO_enableExtIrq(IO_num_e num);
extern bool IO_setExtIrqCallbackFn(IO_num_e num, IO_ext_irq_callback_fn fn);


#endif
