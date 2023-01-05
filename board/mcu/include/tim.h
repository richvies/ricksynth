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


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"


#define EMPTY
#define TIMEOUT(wait_condition, ms, task, pass, fail)   \
{                                             \
  bool p = true;                              \
  uint32_t tim = TIM_millis();                \
                                              \
  task;                                       \
  while(wait_condition)                       \
  {                                           \
    task;                                     \
    if ((TIM_millis() - tim) > ms)            \
    {                                         \
      p = false;                              \
      break;                                  \
    }                                         \
  }                                           \
  if (p) {                                    \
    pass; }                                   \
  else {                                      \
    fail; }                                   \
}


typedef enum
{
  TIM_MODE_TIMER,
  TIM_MODE_OUTPUT_COMPARE,
  TIM_MODE_INPUT_CAPTURE,
} TIM_mode_e;

typedef enum
{
  TIM_DIR_UP,
  TIM_DIR_DOWN,
  TIM_DIR_BOTH,
} TIM_dir_e;

typedef enum
{
  TIM_INPUT_FILTER_OFF,
  TIM_INPUT_FILTER_LOW,
  TIM_INPUT_FILTER_MID,
  TIM_INPUT_FILTER_HIGH,
} TIM_input_filter_e;


typedef struct
{
  bool error;
  TIM_sub_ch_e sub_ch;
  union
  {
    struct
    {
      uint16_t count;
    } capture;
    struct
    {
      bool isHigh;
    } compare;
  };
} TIM_cb_args_t;

typedef void (*TIM_cb)(TIM_cb_args_t *args);
typedef void (*ALARM_cb)(void);

typedef struct
{
  uint16_t ns_per_tick;
  TIM_mode_e mode;
  TIM_dir_e dir;
  uint16_t compare;
  TIM_cb cb;
} TIM_cfg_t;

typedef struct
{
  uint32_t period_ms;
  bool repeat;
  ALARM_cb cb;
} ALARM_cfg_t;


extern void     TIM_initSystemTimer(void);

extern bool     TIM_init(TIM_ch_e ch, TIM_cfg_t *cfg);
extern bool     TIM_start(TIM_ch_e ch);
extern bool     TIM_stop(TIM_ch_e ch);

extern void     TIM_delayUs(uint32_t delay);
extern void     TIM_delayMs(uint32_t delay);
extern uint32_t TIM_millis(void);

extern bool     ALARM_start(ALARM_ch_e ch, ALARM_cfg_t *cfg);
extern void     ALARM_stop(ALARM_ch_e ch);


#ifdef __cplusplus
}
#endif


#endif
