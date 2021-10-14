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

#include "bsp_dwt.h"
#include "bsp_eeprom.h"

/* 函数定义 */
void bsp_Init(void);
void bsp_RunPer1ms(void);

#endif