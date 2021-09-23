/*
*********************************************************************************************************
*    @Brief   : 数据观察点与跟踪(DWT)模块
*    @Name    : bsp_dwt.h
*    @Version : V1.0
*    @Note    : DWT驱动
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#ifndef __BSP_DWT_H
#define __BSP_DWT_H

/* 宏定义 */
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DBGMCU_CR   *(volatile unsigned int *)0xE0042004

/* 函数声明*/
void bsp_InitDWT(void);
void bsp_DelayUS(uint32_t _ulDelayTime);
void bsp_DelayMS(uint32_t _ulDelayTime);

#endif
