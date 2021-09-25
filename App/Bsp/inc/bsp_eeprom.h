/*
*********************************************************************************************************
*    @Brief   : EEPROM驱动
*    @Name    : bsp_eeprom.h
*    @Version : V1.0
*    @Note    : EEPROM驱动 读写系统参数
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H

uint8_t  bsp_EEReadByte  (uint16_t ReadAddr);
void     bsp_EEWriteByte (uint16_t WriteAddr, uint8_t DataToWrite);
void     bsp_EEWriteWord (uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len);
uint32_t bsp_EEReadWord  (uint16_t ReadAddr, uint8_t Len);
void     bsp_EERead      (uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead);
void     bsp_EEWrite     (uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);

#endif
