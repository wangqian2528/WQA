/******************************************************************************
 * @brief    显示模块头文件汇总
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef enum
{
    /*保留*/
    PAGE_NONE = 0,
    /*用户页面*/
    PAGE_START, //开机画面
    PAGE_HOME,  //主页
    PAGE_SET,   //设置
    PAGE_CAL,   //校准
    PAGE_MOR,   //维护
    PAGE_INFO,  //信息
    /*保留*/
    PAGE_MAX
} Page_Type;

#define IS_PAGE(page) ((page) < (PAGE_MAX) && (page) > (PAGE_NONE))

typedef struct _page_type_struct
{
    Page_Type pageID;
    void (*setupCallback)(void);
    void (*exitCallback)(void);
    struct _page_type_struct *next;
} Page_TypeDef;

typedef struct
{
    Page_Type nowPage;
    Page_Type lastPage;
    Page_Type nextPage;
    Page_TypeDef *head;
} PageManager_TypeDef;

void Page_Register(Page_TypeDef *node);
void Page_Init(void);
Page_TypeDef *Page_GetObj(Page_Type page_id);
void Page_ChangeTo(Page_Type page_id);
void Page_Running(void);

void PageRegister_Start(Page_Type pageID);
void PageRegister_Home(Page_Type pageID);
void PageRegister_Info(Page_Type pageID);

void PageRegister_Set(Page_Type pageID);

#include "lvgl/lvgl.h"

lv_obj_t *AppWindow_GetCont(uint8_t pageID);
lv_coord_t AppWindow_GetHeight(void);
lv_coord_t AppWindow_GetWidth(void);
void AppWindow_Create(void);

#endif // !_DISPLAY_H