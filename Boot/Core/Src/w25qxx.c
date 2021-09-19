#include "w25qxx.h"
#include "quadspi.h"

static uint8_t W25QXX_QPI_MODE = 0;

static uint8_t QSPI_Wait_Flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
    uint8_t flagsta = 0;
    while(wtime)
    {
        flagsta = (QUADSPI->SR & flag) ? 1 : 0;
        if(flagsta == sta)break;
        wtime--;
    }
    if(wtime)return 0;
    else return 1;
}

static void QSPI_Send_CMD(uint8_t cmd, uint32_t addr, uint8_t mode, uint8_t dmcycle)
{
    uint32_t tempreg = 0;
    uint8_t status;
    if(QSPI_Wait_Flag(1 << 5, 0, 0XFFFF) == 0)
    {
        tempreg = 0 << 31;
        tempreg |= 0 << 28;
        tempreg |= 0 << 26;
        tempreg |= ((uint32_t)mode >> 6) << 24;
        tempreg |= (uint32_t)dmcycle << 18;
        tempreg |= ((uint32_t)(mode >> 4) & 0X03) << 12;
        tempreg |= ((uint32_t)(mode >> 2) & 0X03) << 10;
        tempreg |= ((uint32_t)(mode >> 0) & 0X03) << 8;
        tempreg |= cmd;
        QUADSPI->CCR = tempreg;
        if(mode & 0X0C)
        {
            QUADSPI->AR = addr;
        }
        if((mode & 0XC0) == 0)
        {
            status = QSPI_Wait_Flag(1 << 1, 1, 0XFFFF);
            if(status == 0)
            {
                QUADSPI->FCR |= 1 << 1;
            }
        }
    }
}

static uint8_t QSPI_Receive(uint8_t *buf, uint32_t datalen)
{
    uint32_t tempreg = QUADSPI->CCR;
    uint32_t addrreg = QUADSPI->AR;
    uint8_t status;
    __IO uint32_t *data_reg = &QUADSPI->DR;
    QUADSPI->DLR = datalen - 1;
    tempreg &= ~(3 << 26);
    tempreg |= 1 << 26;
    QUADSPI->FCR |= 1 << 1;
    QUADSPI->CCR = tempreg;
    QUADSPI->AR = addrreg;
    while(datalen)
    {
        status = QSPI_Wait_Flag(3 << 1, 1, 0XFFFF);
        if(status == 0)
        {
            *buf++ = *(__IO uint8_t *)data_reg;
            datalen--;
        }
        else break;
    }
    if(status == 0)
    {
        QUADSPI->CR |= 1 << 2;
        status = QSPI_Wait_Flag(1 << 1, 1, 0XFFFF);
        if(status == 0)
        {
            QUADSPI->FCR |= 1 << 1;
            status = QSPI_Wait_Flag(1 << 5, 0, 0XFFFF);
        }
    }
    return status;
}

static uint8_t QSPI_Transmit(uint8_t *buf, uint32_t datalen)
{
    uint32_t tempreg = QUADSPI->CCR;
    uint32_t addrreg = QUADSPI->AR;
    uint8_t status;
    __IO uint32_t *data_reg = &QUADSPI->DR;
    QUADSPI->DLR = datalen - 1;
    tempreg &= ~(3 << 26);
    tempreg |= 0 << 26;
    QUADSPI->FCR |= 1 << 1;
    QUADSPI->CCR = tempreg;
    while(datalen)
    {
        status = QSPI_Wait_Flag(1 << 2, 1, 0XFFFF);
        if(status != 0)
        {
            break;
        }
        *(__IO uint8_t *)data_reg = *buf++;
        datalen--;
    }
    if(status == 0)
    {
        QUADSPI->CR |= 1 << 2;
        status = QSPI_Wait_Flag(1 << 1, 1, 0XFFFF);
        if(status == 0)
        {
            QUADSPI->FCR |= 1 << 1;
            status = QSPI_Wait_Flag(1 << 5, 0, 0XFFFF);
        }
    }
    return status;
}

static uint8_t W25QXX_ReadSR(uint8_t regno)
{
    uint8_t byte = 0, command = 0;
    switch(regno)
    {
    case 1:
        command = W25X_ReadStatusReg1;
        break;
    case 2:
        command = W25X_ReadStatusReg2;
        break;
    case 3:
        command = W25X_ReadStatusReg3;
        break;
    default:
        command = W25X_ReadStatusReg1;
        break;
    }
    if(W25QXX_QPI_MODE)QSPI_Send_CMD(command, 0, (3 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
    else QSPI_Send_CMD(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    QSPI_Receive(&byte, 1);
    return byte;
}

static void W25QXX_Write_SR(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;
    switch(regno)
    {
    case 1:
        command = W25X_WriteStatusReg1;
        break;
    case 2:
        command = W25X_WriteStatusReg2;
        break;
    case 3:
        command = W25X_WriteStatusReg3;
        break;
    default:
        command = W25X_WriteStatusReg1;
        break;
    }
    if(W25QXX_QPI_MODE)QSPI_Send_CMD(command, 0, (3 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
    else QSPI_Send_CMD(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    QSPI_Transmit(&sr, 1);
}

static void W25QXX_Write_Enable(void)
{
    if(W25QXX_QPI_MODE)QSPI_Send_CMD(W25X_WriteEnable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
    else QSPI_Send_CMD(W25X_WriteEnable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

static void W25QXX_Qspi_Enable(void)
{
    uint8_t stareg2;
    stareg2 = W25QXX_ReadSR(2);
    if((stareg2 & 0X02) == 0)
    {
        W25QXX_Write_Enable();
        stareg2 |= 1 << 1;
        W25QXX_Write_SR(2, stareg2);
    }
    QSPI_Send_CMD(W25X_EnterQPIMode, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    W25QXX_QPI_MODE = 1;
}

void W25QXX_EnableMemoryMapped(void)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* 配置读指令 */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = 0XEB;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 8;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* 配置内存映射模式 */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    while(1);
  }
}

void W25QXX_Init(void)
{
    uint8_t temp;
    W25QXX_Qspi_Enable();
    temp = W25QXX_ReadSR(3);
    if((temp & 0X01) == 0)
    {
        W25QXX_Write_Enable();
        QSPI_Send_CMD(W25X_Enable4ByteAddr, 0, (0 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
    }
    W25QXX_Write_Enable();
    QSPI_Send_CMD(W25X_SetReadParam, 0, (3 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
    temp = 3 << 4;
    QSPI_Transmit(&temp, 1);
}
