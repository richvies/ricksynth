/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DAC_CS_Pin LL_GPIO_PIN_13
#define DAC_CS_GPIO_Port GPIOC
#define Pots_Pin LL_GPIO_PIN_0
#define Pots_GPIO_Port GPIOA
#define AI_O1_Pin LL_GPIO_PIN_1
#define AI_O1_GPIO_Port GPIOA
#define AI_O2_Pin LL_GPIO_PIN_2
#define AI_O2_GPIO_Port GPIOA
#define AI_O3_Pin LL_GPIO_PIN_3
#define AI_O3_GPIO_Port GPIOA
#define AI_O4_Pin LL_GPIO_PIN_4
#define AI_O4_GPIO_Port GPIOA
#define EAI_O1_Pin LL_GPIO_PIN_0
#define EAI_O1_GPIO_Port GPIOB
#define EAI_O2_Pin LL_GPIO_PIN_1
#define EAI_O2_GPIO_Port GPIOB
#define EAI_O3_Pin LL_GPIO_PIN_2
#define EAI_O3_GPIO_Port GPIOB
#define EAI_O4_Pin LL_GPIO_PIN_10
#define EAI_O4_GPIO_Port GPIOB
#define Shift_Reg_CS_Pin LL_GPIO_PIN_8
#define Shift_Reg_CS_GPIO_Port GPIOA
#define Midi_TX__I_O5_Pin LL_GPIO_PIN_9
#define Midi_TX__I_O5_GPIO_Port GPIOA
#define Midi_I_O6_Pin LL_GPIO_PIN_10
#define Midi_I_O6_GPIO_Port GPIOA
#define Unavailable_Pin LL_GPIO_PIN_13
#define Unavailable_GPIO_Port GPIOA
#define UnavailableA14_Pin LL_GPIO_PIN_14
#define UnavailableA14_GPIO_Port GPIOA
#define Flash_CS_Pin LL_GPIO_PIN_8
#define Flash_CS_GPIO_Port GPIOB
#define Alt_CS_Pin LL_GPIO_PIN_9
#define Alt_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
