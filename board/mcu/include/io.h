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


#ifdef __cplusplus
 extern "C" {
#endif


#include "mcu.h"


/* io pins represented as 16 bit number (8 for port, 8 for pin number):
 *
 *    port no.  pin no.
 * 0b|00000001|00000011|  = port 1, pin 3
 *    -------- --------
 */
#define IO_portPinToNum(port, pin)  (((IO_num_e)port << 8) | (uint8_t)pin)

/* to represent that no pin is used
 * e.g. if spi cs or usart cts pin is not used */
#define IO_NULL_PIN                 (0)


typedef uint16_t IO_num_e;


typedef enum
{
  IO_MODE_GPIO_IN,
  IO_MODE_GPIO_OUT_PP,
  IO_MODE_GPIO_OUT_OD,
  IO_MODE_PERIPH_IN,
  IO_MODE_PERIPH_OUT_PP,
  IO_MODE_PERIPH_OUT_OD,
  IO_MODE_ANALOG,
  IO_MODE_IRQ_POS,
  IO_MODE_IRQ_NEG,
  IO_MODE_IRQ_BOTH,
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

typedef enum
{
  IO_IRQ_EDGE_POS,
  IO_IRQ_EDGE_NEG,
} IO_irq_edge_e;


typedef struct
{
  IO_mode_e       mode;
  IO_speed_e      speed;
  IO_pull_res_e   pullup;
  void const *    extend;
} IO_cfg_t;


typedef void (*IO_ext_irq_cb)(IO_irq_edge_e edge);


extern void IO_init(void);
extern void IO_configure(IO_num_e num, IO_cfg_t *cfg);
extern void IO_deinit(IO_num_e num);

/* num must be > 0 */
extern void IO_set(IO_num_e num);
extern void IO_clear(IO_num_e num);
extern void IO_toggle(IO_num_e num);
extern bool IO_isHigh(IO_num_e num);

extern bool IO_enableExtIrq(IO_num_e num);
extern bool IO_disableExtIrq(IO_num_e num);
extern bool IO_setExtIrqCallback(IO_num_e num, IO_ext_irq_cb fn);


#ifdef __cplusplus
}
#endif


#endif
