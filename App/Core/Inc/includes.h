/******************************************************************************
 * @brief    头文件汇总
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

/*
*********************************************************************************************************
*                                           通用宏定义
*********************************************************************************************************
*/
#define ANONY_CONN(type, var, line) type var##line
#define ANONY_DEF(type, prefix, line) ANONY_CONN(type, prefix, line)
#define ANONY_TYPE(type, prefix) ANONY_DEF(type, prefix, __LINE__)
#define SECTION(x) @x
#define USED __root

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x) * (x))

/**
  * @brief  变量监视器，当变量改变时触发一个事件
  * @param  now:被监控的变量(整形)
  * @param  func:事件触发回调函数(可以是赋值等其他语句)
  * @retval 无
  */
#define __ValueMonitor(now, func) \
  do                              \
  {                               \
    static int last = (now);      \
    if (last != (now))            \
      func, last = (now);         \
  } while (0)

/**
  * @brief  让一个变量以设计的步近接近指定值
  * @param  src:被控变量
  * @param  dest:被接近的值
  * @param  step:步长
  * @retval 无
  */
#define __ValueCloseTo(src, dest, step) \
  do                                    \
  {                                     \
    if ((src) < (dest))                 \
      (src) += (step);                  \
    else if ((src) > (dest))            \
      (src) -= (step);                  \
  } while (0)

/**
  * @brief  让一个变量增加或者减去一个值，在大于等于最大值后从0开始，低于0后从最大值开始
  * @param  src:被控变量
  * @param  step:增加或者减少的值
  * @param  max:最大值
  * @retval 无
  */
#define __ValueStep(src, step, max) ((src) = (((step) >= 0) ? (((src) + (step)) % (max)) : (((src) + (max) + (step)) % (max))))

/**
  * @brief  让一个变量增加或者减去一个值，在大于最大值后从最小值开始，小于最小值后从最大值开始
  * @param  src:被控变量
  * @param  plus:增加的值
  * @param  min:最小值
  * @param  max:最大值
  * @retval 无
  */
#define __ValuePlus(src, plus, min, max) \
  do                                     \
  {                                      \
    int __value_temp = (src);            \
    __value_temp += (plus);              \
    if (__value_temp < (min))            \
      __value_temp = (max);              \
    else if (__value_temp > (max))       \
      __value_temp = (min);              \
    (src) = __value_temp;                \
  } while (0)

/**
  * @brief  将一个函数重复调用指定次数
  * @param  func:被调用函数(可以是赋值等其他语句)
  * @param  n:重复调用次数
  * @retval 无
  */
#define __LoopExecute(func, n)            \
  for (unsigned long i = 0; i < (n); i++) \
  func

/**
  * @brief  将一个值限制在一个范围内
  * @param  x:被限制的值(任意类型)
  * @param  min:最小值(任意类型)
  * @param  max:最大值(任意类型)
  * @retval 无
  */
#define __LimitValue(x, min, max) ((x) = constrain((x), (min), (max)))

/**
  * @brief  将一个值的变化区间线性映射到另一个区间
  * @param  x:被映射的值(任意类型)
  * @param  in_min:被映射的值的最小值
  * @param  in_min:被映射的值的最大值
  * @param  out_min:被映射的值的最小值
  * @param  out_min:被映射的值的最大值
  * @retval 映射值输出
  */
#define __Map(x, in_min, in_max, out_min, out_max) \
  (((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

/**
  * @brief  获取一个数组的元素个数
  * @param  arr:数组名(任意类型)
  * @retval 这个数组的元素个数
  */
#define __Sizeof(arr) (sizeof(arr) / sizeof(arr[0]))

/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/*
*********************************************************************************************************
*                                         STM32
*********************************************************************************************************
*/
#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"

#include "tim.h"
#include "adc.h"
#include "sdmmc.h"
#include "rtc.h"
#include "gpio.h"
#include "usart.h"
#include "display.h"

/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/
#include "tx_api.h"
#include "tx_timer.h"
#include "fx_api.h"

/*
*********************************************************************************************************
*                                          GUI
*********************************************************************************************************
*/
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "lv_demo_widgets/lv_demo_widgets.h"
/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/
#include "bsp.h"
#include "bsp_dwt.h"
#include "bsp_eeprom.h"
#include "bsp_cli.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_adc.h"
#include "bsp_sdio.h"
#include "bsp_ringbuffer.h"

/*
*********************************************************************************************************
*                                        变量和函数
*********************************************************************************************************
*/

extern void App_Printf(const char *fmt, ...);

#endif