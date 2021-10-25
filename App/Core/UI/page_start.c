/******************************************************************************
 * @brief    开机页面
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

/*此页面窗口*/
static lv_obj_t *appWindow;

/*标题信息*/
static lv_obj_t *labelTitle;

/*启动进度条*/
static lv_obj_t *barStart;

/*页面跳转定时器*/
static lv_timer_t *tmrPageJump;

/*显示字符串*/
const char *COMPANY_NAME = "Hello World!";

/*
 ********************************************************************************************************
 *@func       LabelTitle_Creat
 *@brief      创建标题
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void LabelTitle_Creat(void)
{
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labelTitle = lv_label_create(appWindow);
    lv_obj_set_style_text_font(labelTitle, &SYHT_BOLD_20, 0);
    lv_obj_set_style_text_color(labelTitle, lv_color_black(), 0);
    lv_label_set_text(labelTitle, COMPANY_NAME);
    lv_obj_align(labelTitle, LV_ALIGN_CENTER, 0, -10);
}

/*
 ********************************************************************************************************
 *@func       BarStart_Creat
 *@brief      创建启动进度条
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void BarStart_Creat(void)
{
    barStart = lv_bar_create(appWindow);

    lv_obj_set_style_bg_color(barStart, lv_color_white(), 0);
    lv_obj_set_style_border_color(barStart, lv_color_black(), 0);
    lv_obj_set_style_border_width(barStart, 2, 0);
    lv_obj_set_style_pad_all(barStart, 6, 0);
    lv_obj_set_style_radius(barStart, 6, 0);
    lv_obj_set_style_anim_time(barStart, 5000, 0);

    lv_obj_set_style_bg_opa(barStart, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(barStart, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(barStart, 3, LV_PART_INDICATOR);

    lv_obj_set_size(barStart, 200, 20);
    lv_obj_align(barStart, LV_ALIGN_CENTER, 0, 20);
    lv_bar_set_value(barStart, 100, LV_ANIM_ON);
}

/*
 ********************************************************************************************************
 *@func       TimerCB_PageJump
 *@brief      定时器任务回调
 *@param[in]  tmr
 *@retval     none
 ********************************************************************************************************
 */
static void TimerCB_PageJump(lv_timer_t *tmr)
{
    Page_ChangeTo(PAGE_HOME);
}

/*
 ********************************************************************************************************
 *@func       Timer_Create
 *@brief      创建任务/定时器
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Timer_Create(void)
{
    tmrPageJump = lv_timer_create(TimerCB_PageJump, 7000, NULL);
}

/*
 ********************************************************************************************************
 *@func       Setup
 *@brief      页面初始化事件
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Setup(void)
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);

    /*创建控件*/
    LabelTitle_Creat();
    BarStart_Creat();

    /*创建定时器任务*/
    Timer_Create();
}

/*
 ********************************************************************************************************
 *@func       Exit
 *@brief      页面退出事件
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Exit(void)
{
    /*关任务*/
    lv_timer_del(tmrPageJump);

    /*删除此页面上的子控件*/
    lv_obj_clean(appWindow);
}

/*
 ********************************************************************************************************
 *@brief      页面控制器
 ********************************************************************************************************
 */
Page_TypeDef page_Start = {
    .pageID = PAGE_START,
    .exitCallback = Exit,
    .setupCallback = Setup,
    .next = NULL,
};

/*
 ********************************************************************************************************
 *@func       PageRegister_Start
 *@brief      页面注册
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
void PageRegister_Start(Page_Type pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    /*注册至页面调度器*/
    Page_Register(&page_Start);
}