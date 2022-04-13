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


#include <stdint.h>
#include "_vectors.h"


#define WEAK_REF_ATTRIBUTE    __attribute__((weak, alias("defaultHandler")))


/* generated by linker */
extern uint8_t _estack;


void defaultHandler(void)
{
}

void NMI_Handler        (void) WEAK_REF_ATTRIBUTE;
void HardFault_Handler  (void) WEAK_REF_ATTRIBUTE;
void MemManage_Handler  (void) WEAK_REF_ATTRIBUTE;
void BusFault_Handler   (void) WEAK_REF_ATTRIBUTE;
void UsageFault_Handler (void) WEAK_REF_ATTRIBUTE;
void SVC_Handler        (void) WEAK_REF_ATTRIBUTE;
void DebugMon_Handler   (void) WEAK_REF_ATTRIBUTE;
void PendSV_Handler     (void) WEAK_REF_ATTRIBUTE;
void SysTick_Handler    (void) WEAK_REF_ATTRIBUTE;

void WWDG_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void PVD_IRQHandler                   (void) WEAK_REF_ATTRIBUTE;
void TAMP_STAMP_IRQHandler            (void) WEAK_REF_ATTRIBUTE;
void RTC_WKUP_IRQHandler              (void) WEAK_REF_ATTRIBUTE;
void FLASH_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void RCC_IRQHandler                   (void) WEAK_REF_ATTRIBUTE;
void EXTI0_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void EXTI1_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void EXTI2_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void EXTI3_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void EXTI4_IRQHandler                 (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream0_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream1_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream2_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream3_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream4_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream5_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream6_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void ADC_IRQHandler                   (void) WEAK_REF_ATTRIBUTE;
void EXTI9_5_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void TIM1_BRK_TIM9_IRQHandler         (void) WEAK_REF_ATTRIBUTE;
void TIM1_UP_TIM10_IRQHandler         (void) WEAK_REF_ATTRIBUTE;
void TIM1_TRG_COM_TIM11_IRQHandler    (void) WEAK_REF_ATTRIBUTE;
void TIM1_CC_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void TIM2_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void TIM3_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void TIM4_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void I2C1_EV_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void I2C1_ER_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void I2C2_EV_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void I2C2_ER_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void SPI1_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void SPI2_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void USART1_IRQHandler                (void) WEAK_REF_ATTRIBUTE;
void USART2_IRQHandler                (void) WEAK_REF_ATTRIBUTE;
void EXTI15_10_IRQHandler             (void) WEAK_REF_ATTRIBUTE;
void RTC_Alarm_IRQHandler             (void) WEAK_REF_ATTRIBUTE;
void OTG_FS_WKUP_IRQHandler           (void) WEAK_REF_ATTRIBUTE;
void DMA1_Stream7_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void SDIO_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void TIM5_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void SPI3_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream0_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream1_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream2_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream3_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream4_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void OTG_FS_IRQHandler                (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream5_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream6_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void DMA2_Stream7_IRQHandler          (void) WEAK_REF_ATTRIBUTE;
void USART6_IRQHandler                (void) WEAK_REF_ATTRIBUTE;
void I2C3_EV_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void I2C3_ER_IRQHandler               (void) WEAK_REF_ATTRIBUTE;
void FPU_IRQHandler                   (void) WEAK_REF_ATTRIBUTE;
void SPI4_IRQHandler                  (void) WEAK_REF_ATTRIBUTE;


irq_vector_t _vectors[] __attribute__ ((section(".irq_vectors"),used)) =
{
  (irq_vector_t)&_estack,         /*      Initial Stack Pointer     */
  START_resetHandler,             /*      Reset Handler             */
  NMI_Handler,                    /*      NMI Handler               */
  HardFault_Handler,              /*      Hard Fault Handler        */
  MemManage_Handler,              /*      MPU Fault Handler         */
  BusFault_Handler,               /*      Bus Fault Handler         */
  UsageFault_Handler,             /*      Usage Fault Handler       */
  0,                              /*      Reserved                  */
  0,                              /*      Reserved                  */
  0,                              /*      Reserved                  */
  0,                              /*      Reserved                  */
  SVC_Handler,                    /*      SVCall Handler            */
  DebugMon_Handler,               /*      Debug Monitor Handler     */
  0,                              /*      Reserved                  */
  PendSV_Handler,                 /*      PendSV Handler            */
  SysTick_Handler,                /*      SysTick Handler           */
  WWDG_IRQHandler,                /* Window WatchDog                */
  PVD_IRQHandler,                 /* PVD through EXTI Line          */
  TAMP_STAMP_IRQHandler,          /* Tamper & TimeStamps EXTI line  */
  RTC_WKUP_IRQHandler,            /* RTC Wakeup through  EXTI line  */
  FLASH_IRQHandler,               /* FLASH                          */
  RCC_IRQHandler,                 /* RCC                            */
  EXTI0_IRQHandler,               /* EXTI Line0                     */
  EXTI1_IRQHandler,               /* EXTI Line1                     */
  EXTI2_IRQHandler,               /* EXTI Line2                     */
  EXTI3_IRQHandler,               /* EXTI Line3                     */
  EXTI4_IRQHandler,               /* EXTI Line4                     */
  DMA1_Stream0_IRQHandler,        /* DMA1 Stream 0                  */
  DMA1_Stream1_IRQHandler,        /* DMA1 Stream 1                  */
  DMA1_Stream2_IRQHandler,        /* DMA1 Stream 2                  */
  DMA1_Stream3_IRQHandler,        /* DMA1 Stream 3                  */
  DMA1_Stream4_IRQHandler,        /* DMA1 Stream 4                  */
  DMA1_Stream5_IRQHandler,        /* DMA1 Stream 5                  */
  DMA1_Stream6_IRQHandler,        /* DMA1 Stream 6                  */
  ADC_IRQHandler,                 /* ADC1                           */
  0,                              /* Reserved                       */
  0,              	              /* Reserved                       */
  0,                              /* Reserved                       */
  0,                              /* Reserved                       */
  EXTI9_5_IRQHandler,             /* External Line[9:5]s            */
  TIM1_BRK_TIM9_IRQHandler,       /* TIM1 Break and TIM9            */
  TIM1_UP_TIM10_IRQHandler,       /* TIM1 Update and TIM10          */
  TIM1_TRG_COM_TIM11_IRQHandler,  /* TIM1 Trigger & Commutation & TIM11 */
  TIM1_CC_IRQHandler,             /* TIM1 Capture Compare         */
  TIM2_IRQHandler,                /* TIM2                         */
  TIM3_IRQHandler,                /* TIM3                         */
  TIM4_IRQHandler,                /* TIM4                         */
  I2C1_EV_IRQHandler,             /* I2C1 Event                   */
  I2C1_ER_IRQHandler,             /* I2C1 Error                   */
  I2C2_EV_IRQHandler,             /* I2C2 Event                   */
  I2C2_ER_IRQHandler,             /* I2C2 Error                   */
  SPI1_IRQHandler,                /* SPI1                         */
  SPI2_IRQHandler,                /* SPI2                         */
  USART1_IRQHandler,              /* USART1                       */
  USART2_IRQHandler,              /* USART2                       */
  0,                              /* Reserved                     */
  EXTI15_10_IRQHandler,           /* External Line[15:10]s        */
  RTC_Alarm_IRQHandler,           /* RTC Alarm (A and B) through EXTI Line */
  OTG_FS_WKUP_IRQHandler,         /* USB OTG FS Wakeup through EXTI line */
  0,                              /* Reserved     				        */
  0,                              /* Reserved       			        */
  0,                              /* Reserved 					          */
  0,                              /* Reserved                     */
  DMA1_Stream7_IRQHandler,        /* DMA1 Stream7                 */
  0,                              /* Reserved                     */
  SDIO_IRQHandler,                /* SDIO                         */
  TIM5_IRQHandler,                /* TIM5                         */
  SPI3_IRQHandler,                /* SPI3                         */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  DMA2_Stream0_IRQHandler,        /* DMA2 Stream 0                */
  DMA2_Stream1_IRQHandler,        /* DMA2 Stream 1                */
  DMA2_Stream2_IRQHandler,        /* DMA2 Stream 2                */
  DMA2_Stream3_IRQHandler,        /* DMA2 Stream 3                */
  DMA2_Stream4_IRQHandler,        /* DMA2 Stream 4                */
  0,                    			    /* Reserved                     */
  0,              					      /* Reserved                     */
  0,              					      /* Reserved                     */
  0,             					        /* Reserved                     */
  0,              					      /* Reserved                     */
  0,              					      /* Reserved                     */
  OTG_FS_IRQHandler,              /* USB OTG FS                   */
  DMA2_Stream5_IRQHandler,        /* DMA2 Stream 5                */
  DMA2_Stream6_IRQHandler,        /* DMA2 Stream 6                */
  DMA2_Stream7_IRQHandler,        /* DMA2 Stream 7                */
  USART6_IRQHandler,              /* USART6                       */
  I2C3_EV_IRQHandler,             /* I2C3 event                   */
  I2C3_ER_IRQHandler,             /* I2C3 error                   */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  FPU_IRQHandler,                 /* FPU                          */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  SPI4_IRQHandler,                /* SPI4                         */
};
