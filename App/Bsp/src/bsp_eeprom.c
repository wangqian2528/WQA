/*
*********************************************************************************************************
*    @Brief   : EEPROM驱动
*    @Name    : bsp_eeprom.c
*    @Version : V1.0
*    @Note    : EEPROM驱动 读写系统参数
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include "bsp.h"

//IO方向设置
#define SDA_IN()                {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;} //PB7输入模式
#define SDA_OUT()               {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB7输出模式
//IO操作函数
#define WP_SET(x)               HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,x) //WP
#define IIC_SCL(x)              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,x) //SCL
#define IIC_SDA(x)              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,x) //SDA
#define READ_SDA                HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) //读取SDA

/*******************************************************************************
 * @brief       产生IIC起始信号
 * @param[in]   none
 * @return      none
 ******************************************************************************/
static void IIC_Start(void)
{
    SDA_OUT();//sda线输出
    IIC_SDA(1);
    IIC_SCL(1);
    bsp_DelayUS(4);
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
    bsp_DelayUS(4);
    IIC_SCL(0);//钳住I2C总线，准备发送或接收数据
}

/*******************************************************************************
 * @brief       产生IIC停止信号
 * @param[in]   none
 * @return      none
 ******************************************************************************/
static void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL(0);
    IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
    bsp_DelayUS(4);
    IIC_SCL(1);
    bsp_DelayUS(4);
    IIC_SDA(1);//发送I2C总线结束信号
}

/*******************************************************************************
 * @brief       等待应答信号到来
 * @param[in]   none
 * @return      1，接收应答失败 0，接收应答成功
 ******************************************************************************/
static uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA(1);
    bsp_DelayUS(1);
    IIC_SCL(1);
    bsp_DelayUS(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0);//时钟输出0
    return 0;
}

/*******************************************************************************
 * @brief       产生ACK应答
 * @param[in]   none
 * @return      none
 ******************************************************************************/
static void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    bsp_DelayUS(2);
    IIC_SCL(1);
    bsp_DelayUS(2);
    IIC_SCL(0);
}

/*******************************************************************************
 * @brief       不产生ACK应答
 * @param[in]   none
 * @return      none
 ******************************************************************************/
static void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    bsp_DelayUS(2);
    IIC_SCL(1);
    bsp_DelayUS(2);
    IIC_SCL(0);
}

/*******************************************************************************
 * @brief       IIC发送一个字节
 * @param[in]   none
 * @return      1，有应答 0，无应答
 ******************************************************************************/
static void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(0);
    for(t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        bsp_DelayUS(2);
        IIC_SCL(1);
        bsp_DelayUS(2);
        IIC_SCL(0);
        bsp_DelayUS(2);
    }
}

/*******************************************************************************
 * @brief       读1个字节，ack
 * @param[in]   ack=1时，发送ACK，ack=0，发送nACK
 * @return      none
 ******************************************************************************/
static uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_IN();
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL(0);
        bsp_DelayUS(2);
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA)receive++;
        bsp_DelayUS(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

/*******************************************************************************
 * @brief       在AT24CXX指定地址读出一个数据
 * @param[in]   ReadAddr:开始读数的地址
 * @return      返回值  :读到的数据
 ******************************************************************************/
uint8_t bsp_EEReadByte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    IIC_Start();
    IIC_Send_Byte(0XA0);
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr >> 8);
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr % 256);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1);
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();
    return temp;
}

/*******************************************************************************
 * @brief       在AT24CXX指定地址写入一个数据
 * @param[in]   WriteAddr  :写入数据的目的地址
                DataToWrite:要写入的数据
 * @return      none
 ******************************************************************************/
void bsp_EEWriteByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
    IIC_Start();
    IIC_Send_Byte(0XA0);
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr >> 8);
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr % 256);
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);
    IIC_Wait_Ack();
    IIC_Stop();
    bsp_DelayMS(10);
}

/*******************************************************************************
 * @brief       在AT24CXX里面的指定地址开始写入长度为Len的数据
 * @param[in]   WriteAddr  :写入数据的目的地址
                DataToWrite:数据数组首地址
                Len        :要写入数据的长度2,4
 * @return      none
 ******************************************************************************/
void bsp_EEWriteWord(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
    uint8_t t;
    for(t = 0; t < Len; t++)
    {
        bsp_EEWriteByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
    }
}

/*******************************************************************************
 * @brief       在AT24CXX里面读出16bit或者32bit的数据
 * @param[in]   WriteAddr  :开始读出的地址
                Len        :要写入数据的长度2,4
 * @return      数据
 ******************************************************************************/
uint32_t bsp_EEReadWord(uint16_t ReadAddr, uint8_t Len)
{
    uint8_t t;
    uint32_t temp = 0;
    for(t = 0; t < Len; t++)
    {
        temp <<= 8;
        temp += bsp_EEReadByte(ReadAddr + Len - t - 1);
    }
    return temp;
}

/*******************************************************************************
 * @brief       在AT24CXX里面的指定地址开始读出指定个数的数据
 * @param[in]   ReadAddr :开始读出的地址
                pBuffer  :数据数组首地址
                NumToRead:要读出数据的个数
 * @return      none
 ******************************************************************************/
void bsp_EERead(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
    while(NumToRead)
    {
        *pBuffer++ = bsp_EEReadByte(ReadAddr++);
        NumToRead--;
    }
}

/*******************************************************************************
 * @brief       在AT24CXX里面的指定地址开始写入指定个数的数据
 * @param[in]   WriteAddr :开始写入的地址
                pBuffer  :数据数组首地址
                NumToWrite:要写入数据的个数
 * @return      none
 ******************************************************************************/
void bsp_EEWrite(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    while(NumToWrite--)
    {
        bsp_EEWriteByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}
