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

#include "_hw_info.h"

#include "tim.h"


/* variables required for stm32 hal library - see mal/system_stm32f4xx.h */
/* cortex core frequency hz */
uint32_t SystemCoreClock = 0;
/* RCC_CFGR possible divider values (f = f0 >> div) */
const uint8_t AHBPrescTable[16] =
{0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t  APBPrescTable[8] =
{0, 0, 0, 0, 1, 2, 3, 4};


static void configClocks(void);
static uint32_t getAhbFreqHz(void);
static uint32_t getApb1FreqHz(void);
static uint32_t getApb2FreqHz(void);


void clk_init(void)
{
  configClocks();
  clk_update();
}

void clk_update(void)
{
  float pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  uint32_t tmp = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P
         */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = ((float)HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = ((float)HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = (float)pllvco/pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}

void clk_periphEnable(PERIPH_e periph)
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

    case PERIPH_GPIO_H:
      __HAL_RCC_GPIOH_CLK_ENABLE();
      break;

    case PERIPH_DMA_1:
      __HAL_RCC_DMA1_CLK_ENABLE();
      break;

    case PERIPH_DMA_2:
      __HAL_RCC_DMA2_CLK_ENABLE();
      break;

    case PERIPH_TIM_5:
      __HAL_RCC_TIM5_CLK_ENABLE();
      break;

    case PERIPH_WWDG:
      __HAL_RCC_WWDG_CLK_ENABLE();
      break;

    case PERIPH_SPI_2:
      __HAL_RCC_SPI2_CLK_ENABLE();
      break;

    case PERIPH_USART_2:
      __HAL_RCC_USART2_CLK_ENABLE();
      break;

    case PERIPH_I2C_1:
      __HAL_RCC_I2C1_CLK_ENABLE();
      break;

    case PERIPH_I2C_2:
      __HAL_RCC_I2C2_CLK_ENABLE();
      break;

    case PERIPH_PWR:
      __HAL_RCC_PWR_CLK_ENABLE();
      break;

    case PERIPH_TIM_1:
      __HAL_RCC_TIM1_CLK_ENABLE();
      break;

    case PERIPH_USART_1:
      __HAL_RCC_USART1_CLK_ENABLE();
      break;

    case PERIPH_USART_6:
      __HAL_RCC_USART6_CLK_ENABLE();
      break;

    case PERIPH_ADC_1:
      __HAL_RCC_ADC1_CLK_ENABLE();
      break;

    case PERIPH_SPI_1:
      __HAL_RCC_SPI1_CLK_ENABLE();
      break;

    case PERIPH_SYSCFG:
      __HAL_RCC_SYSCFG_CLK_ENABLE();
      break;

    case PERIPH_TIM_9:
      __HAL_RCC_TIM9_CLK_ENABLE();
      break;

    case PERIPH_TIM_11:
      __HAL_RCC_TIM11_CLK_ENABLE();
      break;

    default:
      PRINTF_WARN("%u unknown", periph);
      break;
  }
}

void clk_periphReset(PERIPH_e periph)
{
  PRINTF_INFO("reset %u", periph);

  switch (periph)
  {
    case PERIPH_GPIO_A:
      __HAL_RCC_GPIOA_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_GPIOA_RELEASE_RESET();
      break;

    case PERIPH_GPIO_B:
      __HAL_RCC_GPIOB_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_GPIOB_RELEASE_RESET();
      break;

    case PERIPH_GPIO_C:
      __HAL_RCC_GPIOC_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_GPIOC_RELEASE_RESET();
      break;

    case PERIPH_GPIO_H:
      __HAL_RCC_GPIOH_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_GPIOH_RELEASE_RESET();
      break;

    case PERIPH_DMA_1:
      __HAL_RCC_DMA1_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_DMA1_RELEASE_RESET();
      break;

    case PERIPH_DMA_2:
      __HAL_RCC_DMA2_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_DMA2_RELEASE_RESET();
      break;

    case PERIPH_TIM_5:
      __HAL_RCC_TIM5_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_TIM5_RELEASE_RESET();
      break;

    case PERIPH_WWDG:
      __HAL_RCC_WWDG_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_WWDG_RELEASE_RESET();
      break;

    case PERIPH_SPI_2:
      __HAL_RCC_SPI2_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_SPI2_RELEASE_RESET();
      break;

    case PERIPH_USART_2:
      __HAL_RCC_USART2_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_USART2_RELEASE_RESET();
      break;

    case PERIPH_I2C_1:
      __HAL_RCC_I2C1_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_I2C1_RELEASE_RESET();
      break;

    case PERIPH_I2C_2:
      __HAL_RCC_I2C2_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_I2C2_RELEASE_RESET();
      break;

    case PERIPH_PWR:
      __HAL_RCC_PWR_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_PWR_RELEASE_RESET();
      break;

    case PERIPH_TIM_1:
      __HAL_RCC_TIM1_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_TIM1_RELEASE_RESET();
      break;

    case PERIPH_USART_1:
      __HAL_RCC_USART1_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_USART1_RELEASE_RESET();
      break;

    case PERIPH_USART_6:
      __HAL_RCC_USART6_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_USART6_RELEASE_RESET();
      break;

    case PERIPH_ADC_1:
      __HAL_RCC_ADC_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_ADC_RELEASE_RESET();
      break;

    case PERIPH_SPI_1:
      __HAL_RCC_SPI1_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_SPI1_RELEASE_RESET();
      break;

    case PERIPH_SYSCFG:
      __HAL_RCC_SYSCFG_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_SYSCFG_RELEASE_RESET();
      break;

    case PERIPH_TIM_9:
      __HAL_RCC_TIM9_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_TIM9_RELEASE_RESET();
      break;

    case PERIPH_TIM_11:
      __HAL_RCC_TIM11_FORCE_RESET();
      TIM_delayMs(10);
      __HAL_RCC_TIM11_RELEASE_RESET();
      break;

    default:
      PRINTF_WARN("%u unknown", periph);
      break;
  }
}

void clk_periphResetAll(void)
{
  HAL_DeInit();
}

uint32_t clk_getPeriphBaseClkHz(PERIPH_e periph)
{
  uint32_t ret = 0;

  switch (periph)
  {
    case PERIPH_GPIO_A:
    case PERIPH_GPIO_B:
    case PERIPH_GPIO_C:
    case PERIPH_GPIO_H:
    case PERIPH_DMA_1:
    case PERIPH_DMA_2:
      ret = getAhbFreqHz();
      break;

    case PERIPH_TIM_5:
    case PERIPH_WWDG:
    case PERIPH_SPI_2:
    case PERIPH_USART_2:
    case PERIPH_I2C_1:
    case PERIPH_I2C_2:
    case PERIPH_PWR:
      ret = getApb1FreqHz();
      break;

    case PERIPH_TIM_1:
    case PERIPH_USART_1:
    case PERIPH_USART_6:
    case PERIPH_ADC_1:
    case PERIPH_SPI_1:
    case PERIPH_SYSCFG:
    case PERIPH_TIM_9:
    case PERIPH_TIM_11:
      ret = getApb2FreqHz();
      break;

    default:
      PRINTF_WARN("%u unknown", periph);
      break;
  }

  return ret;
}


static void configClocks(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PLLI2SInitTypeDef RCC_i2sClkInitStruct;

  /* Enable power control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device
  is clocked below the maximum system frequency. To update the voltage scaling
  value according to system frequency, refer to the product datasheet. */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /* Enable HSE oscillator and activate PLL with HSE as source (25 MHz) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;  /* 25 MHz */
  RCC_OscInitStruct.PLL.PLLM = 25;                      /* 25 / 25 = 1 */

  /* PLL1  */
  RCC_OscInitStruct.PLL.PLLN = 336;                     /* 1 * 336 = 336 */
  /* Periph Clk 84 MHz */
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;           /* 336 / 4 = 84 */
  /* USB clock 48 MHz */
  RCC_OscInitStruct.PLL.PLLQ = 7;                       /* 336 / 7 = 48 */

  if (HAL_OK != HAL_RCC_OscConfig(&RCC_OscInitStruct))
  {
    while (1);
  }

  /* PLL2 - I2S */
  /* I2S 2ch * 16 bit * 96000 Hz = 3072000 Hz clock must be multiple of this */
  RCC_i2sClkInitStruct.PLLI2SN = 100;                   /* 1 * 100 = 100 */
  RCC_i2sClkInitStruct.PLLI2SR = 2;                     /* 100 / 2 = 50 */
  /* 50 Mhz -> 97656 HZ sampling frequency */

  if (HAL_OK != HAL_RCCEx_EnablePLLI2S(&RCC_i2sClkInitStruct))
  {
    while (1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_HCLK   |
                                 RCC_CLOCKTYPE_PCLK1  |
                                 RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_OK != HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_ACR_LATENCY_2WS))
  {
    while (1);
  }
}

static uint32_t getAhbFreqHz(void)
{
  uint8_t idx = ((RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos);

  return SystemCoreClock >> AHBPrescTable[idx];
}

static uint32_t getApb1FreqHz(void)
{
  uint8_t idx = ((RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos);

  return SystemCoreClock >> APBPrescTable[idx];
}

static uint32_t getApb2FreqHz(void)
{
  uint8_t idx = ((RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos);

  return SystemCoreClock >> APBPrescTable[idx];
}
