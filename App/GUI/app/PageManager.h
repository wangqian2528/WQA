/******************************************************************************
 * @brief    页面调度器模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*CallbackFunction_t)(void);
typedef void (*EventFunction_t)(void *, int);
typedef struct
{
    CallbackFunction_t SetupCallback;
    CallbackFunction_t LoopCallback;
    CallbackFunction_t ExitCallback;
    EventFunction_t EventCallback;
} PageList_TypeDef;

typedef struct
{

} PageManager_TypeDef;

#endif // !_PAGE_MANAGER_H