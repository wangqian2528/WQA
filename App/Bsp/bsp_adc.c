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

#include "bsp.h"

float g_power_voltage;

/*
 ********************************************************************************************************
 * @func        bsp_adc_RunPeriod
 * @brief       adc检测周期函数
 * @param[in]   period - 周期
 * @return      none
 ********************************************************************************************************
 */
void bsp_adc_RunPeriod(uint32_t period)
{
    static int adc_tick = 0;
    if (adc_tick >= ADC_READ_PERIOD)
    {
        adc_tick = 0;
        for (char n = 0; n < 10; n++)
        {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, 1);
            if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
            {
                g_power_voltage += HAL_ADC_GetValue(&hadc1);
            }
        }
        g_power_voltage = g_power_voltage / 10 / 65536 * 2 * 3.3;
    }
    else
        adc_tick += period;
}

/*
 * @brief      电源电压ADC cmd
 */
static int do_adc_get(uint32_t argc, char *argv[])
{
    printf("power voltage : %.4f V\r\n", g_power_voltage);
    return 1;
}
cmd_register("adc", do_adc_get, "get power voltage");