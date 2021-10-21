/******************************************************************************
 * @brief    SD卡模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _BSP_SDIO_H
#define _BSP_SDIO_H

#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

/**
  * @brief  SD status structure definition
  */
#define MSD_OK ((uint8_t)0x00)
#define MSD_ERROR ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT ((uint8_t)0x02)

/**
  * @brief  SD transfer state definition
  */
#define SD_TRANSFER_OK ((uint8_t)0x00)
#define SD_TRANSFER_BUSY ((uint8_t)0x01)

/** @defgroup STM32H743I_EVAL_SD_Exported_Constants SD Exported Constants
  * @{
  */
#define SD_PRESENT ((uint8_t)0x01)
#define SD_NOT_PRESENT ((uint8_t)0x00)

#define SD_DATATIMEOUT ((uint32_t)100000000)

uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_DeInit(void);

uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t BSP_SD_GetCardState(void);
void BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo);
uint8_t BSP_SD_IsDetected(void);

void BSP_SD_AbortCallback(void);
void BSP_SD_WriteCpltCallback(void);
void BSP_SD_ReadCpltCallback(void);
void BSP_SD_ErrorCallback(void);
void BSP_SD_DriveTransciver_1_8V_Callback(FlagStatus status);

#endif // !_BSP_SDIO_H