/******************************************************************************
 * @brief    电阻触摸屏模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _BSP_TOUCH_H
#define _BSP_TOUCH_H

#define TP_PRES_DOWN 0x8000 //触屏被按下
#define TP_CATH_PRES 0x4000 //有按键按下

#define CT_MAX_TOUCH 1 //电容屏5, 电阻屏1

#define READ_TIMES 5     //读取次数
#define LOST_VAL 1       //丢弃值
#define ERR_RANGE 50     //误差范围
#define SAVE_ADDR_BASE 0 //EEPROM基址 占用12个字节

#define TOUCH_READ_PERIOD 20

#define CMD_RDX 0XD0
#define CMD_RDY 0X90

#define ABS(x, y) ((x) > (y) ? (x - y) : (y - x))

//电阻屏芯片连接引脚
#define PEN HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)         //T_PEN
#define DOUT HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5)        //T_MISO
#define TDIN(x) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, x) //T_MOSI
#define TCLK(x) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, x) //T_SCK
#define TCS(x) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, x)  //T_CS

extern uint16_t g_touch_x[CT_MAX_TOUCH]; //当前坐标
extern uint16_t g_touch_y[CT_MAX_TOUCH];
extern uint16_t g_touch_sta; //笔的状态

void bsp_InitTOUCH(void);
void bsp_touch_adjust(void);
uint8_t bsp_touch_Scan(uint8_t tp);
void bsp_touch_RunPeriod(uint32_t period);

#endif // !_BSP_TOUCH_H