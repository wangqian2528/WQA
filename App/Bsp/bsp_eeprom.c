/******************************************************************************
 * @brief    EEPROM模块
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

/*
*********************************************************************************************************
*                                             变量
*********************************************************************************************************
*/
float g_sys_para_touch_x_fac;//触摸屏X校准参数
float g_sys_para_touch_y_fac;//触摸屏Y校准参数
int16_t g_sys_para_touch_x_off;//触摸屏X校准偏移量
int16_t g_sys_para_touch_y_off;//触摸屏Y校准偏移量
uint8_t g_sys_para_touch_flg;//触摸屏校准标志








//IO方向设置
#define SDA_IN()                         \
    {                                    \
        GPIOB->MODER &= ~(3 << (7 * 2)); \
        GPIOB->MODER |= 0 << 7 * 2;      \
    } //PB7输入模式
#define SDA_OUT()                        \
    {                                    \
        GPIOB->MODER &= ~(3 << (7 * 2)); \
        GPIOB->MODER |= 1 << 7 * 2;      \
    } //PB7输出模式

//IO操作函数
#define WP_SET(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, x)  //WP
#define IIC_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, x) //SCL
#define IIC_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, x) //SDA
#define READ_SDA HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)       //读取SDA

/*
 ********************************************************************************************************
 * @func        IIC_Start
 * @brief       产生IIC起始信号
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
static void IIC_Start(void)
{
    SDA_OUT(); //sda线输出
    IIC_SDA(1);
    IIC_SCL(1);
    bsp_DelayUS(4);
    IIC_SDA(0); //START:when CLK is high,DATA change form high to low
    bsp_DelayUS(4);
    IIC_SCL(0); //钳住I2C总线，准备发送或接收数据
}

/*
 ********************************************************************************************************
 * @func        IIC_Stop
 * @brief       产生IIC停止信号
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
static void IIC_Stop(void)
{
    SDA_OUT(); //sda线输出
    IIC_SCL(0);
    IIC_SDA(0); //STOP:when CLK is high DATA change form low to high
    bsp_DelayUS(4);
    IIC_SCL(1);
    bsp_DelayUS(4);
    IIC_SDA(1); //发送I2C总线结束信号
}

/*
 ********************************************************************************************************
 * @func        IIC_Wait_Ack
 * @brief       产生IIC停止信号
 * @param[in]   none
 * @return      1，接收应答失败 0，接收应答成功
 ********************************************************************************************************
 */
static uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN(); //SDA设置为输入
    IIC_SDA(1);
    bsp_DelayUS(1);
    IIC_SCL(1);
    bsp_DelayUS(1);
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0); //时钟输出0
    return 0;
}

/*
 ********************************************************************************************************
 * @func        IIC_Ack
 * @brief       产生ACK应答
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
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

/*
 ********************************************************************************************************
 * @func        IIC_NAck
 * @brief       不产生ACK应答
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
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

/*
 ********************************************************************************************************
 * @func        IIC_Send_Byte
 * @brief       IIC发送一个字节
 * @param[in]   txd - 发送字节
 * @return      none
 ********************************************************************************************************
 */
static void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(0); //拉低时钟开始数据传输
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        bsp_DelayUS(2); //对TEA5767这三个延时都是必须的
        IIC_SCL(1);
        bsp_DelayUS(2);
        IIC_SCL(0);
        bsp_DelayUS(2);
    }
}

/*
 ********************************************************************************************************
 * @func        IIC_Read_Byte
 * @brief       IIC读一个字节
 * @param[in]   ack=1时，发送ACK，ack=0，发送nACK
 * @return      读到的字节
 ********************************************************************************************************
 */
static uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); //SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        IIC_SCL(0);
        bsp_DelayUS(2);
        IIC_SCL(1);
        receive <<= 1;
        if (READ_SDA)
            receive++;
        bsp_DelayUS(1);
    }
    if (!ack)
        IIC_NAck(); //发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}

/*
 ********************************************************************************************************
 * @func        bsp_eeprom_read_byte
 * @brief       在AT24CXX指定地址读出一个数据
 * @param[in]   ReadAddr:开始读数的地址
 * @return      读到的数据
 ********************************************************************************************************
 */
uint8_t bsp_eeprom_read_byte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    IIC_Start();
    IIC_Send_Byte(0XA0); //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr >> 8); //发送高地址
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr % 256); //发送低地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1); //进入接收模式
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop(); //产生一个停止条件
    return temp;
}

/*
 ********************************************************************************************************
 * @func        bsp_eeprom_write_byte
 * @brief       在AT24CXX指定地址写入一个数据
 * @param[in]   WriteAddr  :写入数据的目的地址
                DataToWrite:要写入的数据
 * @return      none
 ********************************************************************************************************
 */
void bsp_eeprom_write_byte(uint16_t WriteAddr, uint8_t DataToWrite)
{
    IIC_Start();
    IIC_Send_Byte(0XA0); //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr >> 8); //发送高地址
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr % 256); //发送低地址
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite); //发送字节
    IIC_Wait_Ack();
    IIC_Stop(); //产生一个停止条件
    bsp_DelayMS(10);
}

/*
 ********************************************************************************************************
 * @func        bsp_eeprom_read
 * @brief       在AT24CXX里面的指定地址开始读出指定个数的数据
 * @param[in]   ReadAddr :开始读出的地址
                pBuffer  :数据数组首地址
                NumToRead:要读出数据的个数
 * @return      none
 ********************************************************************************************************
 */
void bsp_eeprom_read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
    while (NumToRead)
    {
        *pBuffer++ = bsp_eeprom_read_byte(ReadAddr++);
        NumToRead--;
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_eeprom_write
 * @brief       在AT24CXX里面的指定地址开始写入指定个数的数据
 * @param[in]   WriteAddr :开始写入的地址
                pBuffer  :数据数组首地址
                NumToWrite:要写入数据的个数
 * @return      none
 ********************************************************************************************************
 */
void bsp_eeprom_write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    while (NumToWrite--)
    {
        bsp_eeprom_write_byte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}