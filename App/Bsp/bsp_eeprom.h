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

    EEPROM_DATA_MAX,
} eeprom_data_e;

#define EEPROM_ADDRESS_BASIC                            (0)
#define EEPROM_ADDRESS_TOUCH_ADJ                        (EEPROM_ADDRESS_BASIC + 0)









#endif // !_BSP_EEPROM_H