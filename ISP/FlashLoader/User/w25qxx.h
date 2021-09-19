#ifndef _W25QXX_H
#define _W25QXX_H

#include "qspi.h"

extern u8 W25QXX_QPI_MODE;

#define W25X_WriteEnable            0x06
#define W25X_WriteDisable           0x04

#define W25X_ReadStatusReg1         0x05
#define W25X_ReadStatusReg2         0x35
#define W25X_ReadStatusReg3         0x15

#define W25X_WriteStatusReg1        0x01
#define W25X_WriteStatusReg2        0x31
#define W25X_WriteStatusReg3        0x11

#define W25X_ReadData               0x03
#define W25X_FastRead               0x0B

#define W25X_PageProgram            0x02

#define W25X_SectorErase            0x20
#define W25X_ChipErase              0xC7

#define W25X_JedecDeviceID          0x90

#define W25X_Enable4ByteAddr        0xB7
#define W25X_Exit4ByteAddr          0xE9

#define W25X_EnterQPIMode           0x38
#define W25X_ExitQPIMode            0xFF

#define W25X_SetReadParam           0xC0

void W25QXX_Qspi_Enable(void);
void W25QXX_Qspi_Disable(void);
void W25QXX_Init(void);
void W25QXX_Read(u8 *pBuffer, u32 ReadAddr, u32 NumByteToRead);
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u32 NumByteToWrite);
void W25QXX_Erase_Chip(void);
void W25QXX_Erase_Sector(u32 Dst_Addr);

#endif
