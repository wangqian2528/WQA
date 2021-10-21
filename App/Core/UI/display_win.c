/******************************************************************************
 * @brief    显示模块窗口
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

typedef struct
{
    lv_obj_t *cont;
    lv_style_t style;
} AppWindow_TypeDef;

static AppWindow_TypeDef appWindow_Grp[PAGE_MAX];

/*
 ********************************************************************************************************
 * @func        AppWindow_GetCont
 * @brief       获取页面分配窗口
 * @param[in]   pageID:为此页面分配的ID号
 * @return      none
 ********************************************************************************************************
 */
lv_obj_t *AppWindow_GetCont(uint8_t pageID)
{
    return (pageID < PAGE_MAX) ? appWindow_Grp[pageID].cont : NULL;
}

/*
 ********************************************************************************************************
 * @func        AppWindow_GetHeight
 * @brief       获取窗口高度
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
lv_coord_t AppWindow_GetHeight(void)
{
    return (lv_obj_get_height(lv_scr_act()));
}

/*
 ********************************************************************************************************
 * @func        AppWindow_GetWidth
 * @brief       获取窗口宽度
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
lv_coord_t AppWindow_GetWidth(void)
{
    return (lv_obj_get_width(lv_scr_act()));
}

/*
 ********************************************************************************************************
 * @func        AppWindow_Create
 * @brief       创建页面窗口
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void AppWindow_Create(void)
{
    for (int i = 0; i < PAGE_MAX; i++)
    {
        appWindow_Grp[i].cont = lv_cont_create(lv_scr_act(), NULL);

        lv_obj_set_size(appWindow_Grp[i].cont, AppWindow_GetWidth(), AppWindow_GetHeight());
        lv_obj_align(appWindow_Grp[i].cont, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_cont_set_fit(appWindow_Grp[i].cont, LV_FIT_NONE);

        appWindow_Grp[i].style = lv_style_plain;
        appWindow_Grp[i].style.body.main_color = LV_COLOR_WHITE;
        appWindow_Grp[i].style.body.grad_color = LV_COLOR_WHITE;
        lv_cont_set_style(appWindow_Grp[i].cont, LV_CONT_STYLE_MAIN, &appWindow_Grp[i].style);
    }
}