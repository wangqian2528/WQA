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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32h7xx_hal.h"

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
#define TP_SCK_Pin GPIO_PIN_2
#define TP_SCK_GPIO_Port GPIOE
#define TP_PEN_Pin GPIO_PIN_3
#define TP_PEN_GPIO_Port GPIOE
#define TP_CS_Pin GPIO_PIN_4
#define TP_CS_GPIO_Port GPIOE
#define TP_DI_Pin GPIO_PIN_5
#define TP_DI_GPIO_Port GPIOE
#define TP_DO_Pin GPIO_PIN_6
#define TP_DO_GPIO_Port GPIOE
#define KEY_PWR_Pin GPIO_PIN_0
#define KEY_PWR_GPIO_Port GPIOA
#define SW_PWR_Pin GPIO_PIN_1
#define SW_PWR_GPIO_Port GPIOA
#define LTDC_BK_Pin GPIO_PIN_7
#define LTDC_BK_GPIO_Port GPIOA
#define LTDC_RST_Pin GPIO_PIN_13
#define LTDC_RST_GPIO_Port GPIOB
#define DBG_TX_Pin GPIO_PIN_8
#define DBG_TX_GPIO_Port GPIOD
#define DBG_RX_Pin GPIO_PIN_9
#define DBG_RX_GPIO_Port GPIOD
#define DBG_DE_Pin GPIO_PIN_11
#define DBG_DE_GPIO_Port GPIOD
#define RS485_DE_Pin GPIO_PIN_8
#define RS485_DE_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define EE_WP_Pin GPIO_PIN_5
#define EE_WP_GPIO_Port GPIOB
#define EE_SCL_Pin GPIO_PIN_6
#define EE_SCL_GPIO_Port GPIOB
#define EE_SDA_Pin GPIO_PIN_7
#define EE_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
