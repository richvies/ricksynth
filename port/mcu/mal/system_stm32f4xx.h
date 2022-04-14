/****************************************************************************

This file declares clock variables used by stm32 hal library
They are defined in clk.c

****************************************************************************/

#ifndef __SYSTEM_STM32F4XX_H
#define __SYSTEM_STM32F4XX_H


extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

extern const uint8_t  AHBPrescTable[16];    /*!< AHB prescalers table values */
extern const uint8_t  APBPrescTable[8];     /*!< APB prescalers table values */


#endif
