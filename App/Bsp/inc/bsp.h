/*
*********************************************************************************************************
*    @Brief   : BSP模块
*    @Name    : bsp.h
*    @Version : V1.0
*    @Note    : 底层驱动
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* 含底层驱动模块*/
#include "bsp_dwt.h"


/* 开关全局中断的宏 */
#define  ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
#define  DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */

/* 函数声明*/
void bsp_Init(void);
void bsp_RunPer1ms(void);

#endif
