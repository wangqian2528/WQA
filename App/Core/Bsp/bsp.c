/******************************************************************************
 * @brief    BSP模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#include "includes.h"

/*
 ********************************************************************************************************
 * @func        bsp_Init
 * @brief       bsp初始化
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_Init(void)
{
    bsp_InitDWT();
    bsp_InitEEPROM();
    bsp_InitCLI();
    bsp_InitLCD();
    bsp_InitTOUCH();
}

/*
 ********************************************************************************************************
 * @func        bsp_RunPer1ms
 * @brief       一些需要周期性处理的事务
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_RunPer1ms(void)
{
    bsp_touch_RunPeriod(1);
    bsp_adc_RunPeriod(1);
    lv_tick_inc(1);
}