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


#include "io.h"

#include "clk.h"
#include "mcu.h"
#include "irq.h"


typedef struct
{
  io_ext_irq_hw_info_t const * hw;
  IO_ext_irq_cb cb;
} ext_irq_info_t;


static ext_irq_info_t ext_irq_info[IO_NUM_OF_EXT_IRQ];

static const uint32_t dir_to_hal[IO_NUM_OF_DIRECTIONS] =
{
  GPIO_MODE_INPUT,
  GPIO_MODE_OUTPUT_PP,
  GPIO_MODE_OUTPUT_OD,
};
static const uint32_t mode_to_hal[IO_NUM_OF_MODES] =
{
  0,
  MODE_ANALOG,
  MODE_AF,
  GPIO_MODE_IT_RISING,
  GPIO_MODE_IT_FALLING,
  GPIO_MODE_IT_RISING_FALLING,
};
static const uint32_t pullup_to_hal[IO_NUM_OF_PULL_MODES] =
{
  GPIO_NOPULL,
  GPIO_PULLDOWN,
  GPIO_PULLUP,
  GPIO_NOPULL,
};
static const uint32_t speed_to_hal[IO_NUM_OF_SPEEDS] =
{
  GPIO_SPEED_FREQ_LOW,
  GPIO_SPEED_FREQ_MEDIUM,
  GPIO_SPEED_FREQ_VERY_HIGH,
};


static void handleExtIrq(void);
static bool ioToExt(IO_num_e num, IO_ext_irq_e *ext_irq);


void IO_init(void)
{
  IO_ext_irq_e i;

  for (i = IO_EXT_IRQ_FIRST; i < IO_NUM_OF_EXT_IRQ; i++)
  {
    ext_irq_info[i].hw = &io_ext_irq_hw_info[i];
  }
}

void IO_configure(IO_num_e num, IO_cfg_t *cfg)
{
  GPIO_InitTypeDef init;

  CLK_periphEnable(io_ports_hw_info[IO_TO_PORT(num)].periph);

  init.Pin   = IO_TO_PIN(num);
  init.Pull  = pullup_to_hal[cfg->pullup];
  init.Speed = speed_to_hal[cfg->speed];
  init.Mode  = (dir_to_hal[cfg->dir] | mode_to_hal[cfg->mode]);
  if ((IO_MODE_PERIPH == cfg->mode)
  &&  (NULL != cfg->extend))
  {
    init.Alternate = ((io_cfg_extend_t const *)cfg->extend)->af_value;
  }

  HAL_GPIO_Init(IO_TO_HAL_INST(num), &init);
}

void IO_deinit(IO_num_e num)
{
  HAL_GPIO_DeInit(IO_TO_HAL_INST(num), IO_TO_PIN(num));
}


void IO_set(IO_num_e num)
{
  HAL_GPIO_WritePin(IO_TO_HAL_INST(num), IO_TO_PIN(num), GPIO_PIN_SET);
}

void IO_clear(IO_num_e num)
{
  HAL_GPIO_WritePin(IO_TO_HAL_INST(num), IO_TO_PIN(num), GPIO_PIN_RESET);
}

void IO_toggle(IO_num_e num)
{
  HAL_GPIO_TogglePin(IO_TO_HAL_INST(num), IO_TO_PIN(num));
}

bool IO_isHigh(IO_num_e num)
{
  bool ret = false;

  if (GPIO_PIN_SET == HAL_GPIO_ReadPin(IO_TO_HAL_INST(num), IO_TO_PIN(num)))
  {
    ret = true;
  }

  return ret;
}


bool IO_enableExtIrq(IO_num_e num)
{
  bool ret = false;
  IO_ext_irq_e ext_irq;
  ext_irq_info_t *info;

  if (true == ioToExt(num, &ext_irq))
  {
    info = &ext_irq_info[ext_irq];
    IRQ_config(info->hw->irq_num, info->hw->priority);
    IRQ_enable(info->hw->irq_num);
    ret = true;
  }

  return ret;
}

bool IO_setExtIrqCallback(IO_num_e num, IO_ext_irq_cb fn)
{
  bool ret = false;
  IO_ext_irq_e ext_irq;

  if (true == ioToExt(num, &ext_irq))
  {
    ext_irq_info[ext_irq].cb = fn;
    ret = true;
  }

  return ret;
}


static void handleExtIrq(void)
{
  uint8_t i;
  IO_num_e io_num;
  uint16_t hal_pin;

  for (i = 0; i < IO_NUM_OF_EXT_IRQ; i++)
  {
    io_num = ext_irq_info[i].hw->io_num;
    hal_pin = IO_TO_PIN(io_num);

    if (__HAL_GPIO_EXTI_GET_FLAG(hal_pin))
    {
      HAL_GPIO_EXTI_IRQHandler(hal_pin);

      if (ext_irq_info[i].cb != NULL)
      {
        // TODO get edge
        ext_irq_info[i].cb(IO_IRQ_EDGE_POS);
      }
    }
  }
}

static bool ioToExt(IO_num_e num, IO_ext_irq_e *ext_irq)
{
  bool ret = false;
  IO_ext_irq_e i;

  for (i = IO_EXT_IRQ_FIRST; i < IO_NUM_OF_EXT_IRQ; i++)
  {
    if (ext_irq_info[i].hw->io_num == num)
    {
      *ext_irq = i;
      ret = true;
    }
  }

  return ret;
}


void EXTI1_IRQHandler(void)
{
  handleExtIrq();
}

void EXTI9_5_IRQHandler(void)
{
  handleExtIrq();
}

void EXTI15_10_IRQHandler(void)
{
  handleExtIrq();
}

