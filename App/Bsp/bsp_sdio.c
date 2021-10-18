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

#include "bsp.h"

#define SD_IS_INSERTED() 1

/*
 ********************************************************************************************************
 * @func        BSP_SD_IsDetected
 * @brief       Detects if SD card is correctly plugged in the memory slot or not.
 * @param[in]   none
 * @return      Returns if SD is detected or not
 ********************************************************************************************************
 */
uint8_t BSP_SD_IsDetected(void)
{
  __IO uint8_t status = SD_PRESENT;

  if (SD_IS_INSERTED())
  {
    status = SD_PRESENT;
  }
  else
  {
    status = SD_NOT_PRESENT;
  }
  return status;
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_Init
 * @brief       Initializes the SD card device.
 * @param[in]   none
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_Init(void)
{
  uint8_t sd_state = MSD_OK;
  MX_SDMMC2_SD_Init();
  return sd_state;
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_DeInit
 * @brief       DeInitializes the SD card device.
 * @param[in]   none
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_DeInit(void)
{
  uint8_t sd_state = MSD_OK;

  if (HAL_SD_DeInit(&hsd2) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  HAL_SD_MspDeInit(&hsd2);

  return sd_state;
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_ReadBlocks
 * @brief       Reads block(s) from a specified address in an SD card, in polling mode.
 * @param[in]   pData: Pointer to the buffer that will contain the data to transmit
 *              ReadAddr: Address from where data is to be read
 *              NumOfBlocks: Number of SD blocks to read
 *              Timeout: Timeout for read operation
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  if (HAL_SD_ReadBlocks(&hsd2, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_WriteBlocks
 * @brief       Writes block(s) to a specified address in an SD card, in polling mode.
 * @param[in]   pData: Pointer to the buffer that will contain the data to transmit
 *              WriteAddr: Address from where data is to be written
 *              NumOfBlocks: Number of SD blocks to write
 *              Timeout: Timeout for read operation
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  if (HAL_SD_WriteBlocks(&hsd2, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_ReadBlocks_DMA
 * @brief       Reads block(s) from a specified address in an SD card, in DMA mode.
 * @param[in]   pData: Pointer to the buffer that will contain the data to transmit
 *              ReadAddr: Address from where data is to be read
 *              NumOfBlocks: Number of SD blocks to read
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
  if (HAL_SD_ReadBlocks_DMA(&hsd2, (uint8_t *)pData, ReadAddr, NumOfBlocks) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_WriteBlocks_DMA
 * @brief       Writes block(s) to a specified address in an SD card, in DMA mode.
 * @param[in]   pData: Pointer to the buffer that will contain the data to transmit
 *              WriteAddr: Address from where data is to be written
 *              NumOfBlocks: Number of SD blocks to write
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
  if (HAL_SD_WriteBlocks_DMA(&hsd2, (uint8_t *)pData, WriteAddr, NumOfBlocks) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_Erase
 * @brief       Erases the specified memory area of the given SD card.
 * @param[in]   StartAddr: Start byte address
 *              EndAddr: End byte address
 * @return      SD status
 ********************************************************************************************************
 */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
  if (HAL_SD_Erase(&hsd2, StartAddr, EndAddr) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_GetCardState
 * @brief       Gets the current SD card data status.
 * @param[in]   none
 * @return      Data transfer state.
 *              @arg  SD_TRANSFER_OK: No data transfer is acting
 *              @arg  SD_TRANSFER_BUSY: Data transfer is acting
 ********************************************************************************************************
 */
uint8_t BSP_SD_GetCardState(void)
{
  return ((HAL_SD_GetCardState(&hsd2) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/*
 ********************************************************************************************************
 * @func        BSP_SD_GetCardInfo
 * @brief       Get SD information about specific SD card.
 * @param[out]  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
 * @return      none
 ********************************************************************************************************
 */
void BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo)
{
  HAL_SD_GetCardInfo(&hsd2, CardInfo);
}

/**
  * @brief BSP SD Abort callbacks
  * @retval None
  */
__weak void BSP_SD_AbortCallback(void)
{
}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(void)
{
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(void)
{
}

/**
  * @brief BSP Error callbacks
  * @retval None
  */
__weak void BSP_SD_ErrorCallback(void)
{
}

/**
  * @brief  BSP SD Transceiver 1.8V Mode Callback.
  */
__weak void BSP_SD_DriveTransciver_1_8V_Callback(FlagStatus status)
{
}

/**
  * @brief SD Abort callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_AbortCallback();
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_ReadCpltCallback();
}

/**
  * @brief Error callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_ErrorCallback();
}

/**
  * @brief  Enable the SD Transceiver 1.8V Mode Callback.
  */
void HAL_SD_DriveTransciver_1_8V_Callback(FlagStatus status)
{
  BSP_SD_DriveTransciver_1_8V_Callback(status);
}