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

#ifndef _BSP_H_
#define _BSP_H_

#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tim.h"
#include "adc.h"
#include "sdmmc.h"
#include "rtc.h"

#include "lvgl.h"

#include "bsp_dwt.h"
#include "bsp_eeprom.h"
#include "bsp_cli.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_adc.h"
#include "bsp_sdio.h"

/* 函数定义 */
void bsp_Init(void);
void bsp_RunPer1ms(void);
extern void App_Printf(const char *fmt, ...);

#endif