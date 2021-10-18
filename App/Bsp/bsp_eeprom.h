/******************************************************************************
 * @brief    EEPROM模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _BSP_EEPROM_H
#define _BSP_EEPROM_H

typedef enum
{
    EEPROM_DATA_TOUCH_ADJ,
    EEPROM_DATA_LCD_BKLIGHT,

    EEPROM_DATA_MAX,
} eeprom_data_e;

#define EEPROM_ADDRESS_BASIC (0)
#define EEPROM_ADDRESS_TOUCH_ADJ (EEPROM_ADDRESS_BASIC + 0)
#define EEPROM_ADDRESS_LCD_BKLIGHT (EEPROM_ADDRESS_BASIC + 13)

extern float g_sys_para_touch_x_fac;   //触摸屏X校准参数
extern float g_sys_para_touch_y_fac;   //触摸屏Y校准参数
extern int16_t g_sys_para_touch_x_off; //触摸屏X校准偏移量
extern int16_t g_sys_para_touch_y_off; //触摸屏Y校准偏移量
extern uint8_t g_sys_para_touch_flg;   //触摸屏校准标志
extern uint8_t g_sys_para_lcd_bklight; //LCD屏背光亮度

void bsp_InitEEPROM(void);
void bsp_eeprom_save(eeprom_data_e index);
void bsp_eeprom_load(eeprom_data_e index);

#endif // !_BSP_EEPROM_H