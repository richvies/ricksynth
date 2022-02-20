/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#define DAC_CS_Pin GPIO_PIN_13
#define DAC_CS_GPIO_Port GPIOC
#define POTS_Pin GPIO_PIN_0
#define POTS_GPIO_Port GPIOA
#define DIO_AI1_Pin GPIO_PIN_1
#define DIO_AI1_GPIO_Port GPIOA
#define DIO_AI2_Pin GPIO_PIN_2
#define DIO_AI2_GPIO_Port GPIOA
#define DIO_AI3_Pin GPIO_PIN_3
#define DIO_AI3_GPIO_Port GPIOA
#define DIO_AI4_Pin GPIO_PIN_4
#define DIO_AI4_GPIO_Port GPIOA
#define ExtDIO_AI1_Pin GPIO_PIN_0
#define ExtDIO_AI1_GPIO_Port GPIOB
#define ExtDIO_AI2_Pin GPIO_PIN_1
#define ExtDIO_AI2_GPIO_Port GPIOB
#define ExtDIO3_Pin GPIO_PIN_2
#define ExtDIO3_GPIO_Port GPIOB
#define ExtDIO4_Pin GPIO_PIN_10
#define ExtDIO4_GPIO_Port GPIOB
#define SR_CS_Pin GPIO_PIN_8
#define SR_CS_GPIO_Port GPIOA
#define ALT_CS_Pin GPIO_PIN_12
#define ALT_CS_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_8
#define FLASH_CS_GPIO_Port GPIOB
#define ALT_CSB9_Pin GPIO_PIN_9
#define ALT_CSB9_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
