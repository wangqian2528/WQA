/******************************************************************************
 * @brief    电源电压ADC模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _BSP_ADC_H
#define _BSP_ADC_H

#define ADC_READ_PERIOD 100

extern float g_power_voltage;

void bsp_adc_RunPeriod(uint32_t period);

#endif // !_BSP_ADC_H