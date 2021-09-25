/*
*********************************************************************************************************
*    @Brief   : 电阻屏触摸驱动
*    @Name    : bsp_touch.h
*    @Version : V1.0
*    @Note    : 电阻屏触摸驱动
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#ifndef _BSP_TOUCH_H
#define _BSP_TOUCH_H

#define TP_PRES_DOWN            0x8000 //触屏被按下
#define TP_CATH_PRES            0x4000 //有按键按下
#define CT_MAX_TOUCH            1 //电容屏5, 电阻屏1
#define READ_TIMES              5 //读取次数
#define LOST_VAL                1 //丢弃值
#define ERR_RANGE               50 //误差范围 
#define CMD_RDX                 0XD0
#define CMD_RDY                 0X90
#define ABS(x,y)                ((x) > (y) ? (x - y) : (y - x))

#define ADDR_SYS_PARA_TOUCH     0

extern uint16_t g_touch_x[CT_MAX_TOUCH];
extern uint16_t g_touch_y[CT_MAX_TOUCH];
extern uint16_t g_touch_sta;

enum
{
    TOUCH_NONE = 0,     /* 无触摸 */
    TOUCH_DOWN = 1,     /* 按下 */
    TOUCH_MOVE = 2,     /* 移动 */
    TOUCH_RELEASE = 3   /* 释放 */
};

uint8_t bsp_TouchScan       (uint8_t tp);
void    bsp_InitTouch       (void);
void    bsp_TouchPutKey     (uint8_t _ucEvent, uint16_t _usX, uint16_t _usY);

#endif
