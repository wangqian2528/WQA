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

/*页面跳转任务*/
static lv_task_t *taskPageJump;

/*显示字符串*/
const char *COMPANY_NAME = "苏州讴劲电子有限公司";

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
    labelTitle = lv_label_create(appWindow, NULL);
    static lv_style_t title_style;
    title_style = *lv_label_get_style(labelTitle, LV_LABEL_STYLE_MAIN);
    title_style.text.font = &SYHT_BOLD_20;
    title_style.text.color = LV_COLOR_BLACK;
    lv_label_set_style(labelTitle, LV_LABEL_STYLE_MAIN, &title_style);
    lv_label_set_text(labelTitle, COMPANY_NAME);
    lv_obj_align(labelTitle, NULL, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_auto_realign(labelTitle, true);
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
    barStart = lv_bar_create(appWindow, NULL);
    static lv_style_t bar_bg_style;
    static lv_style_t bar_indic_style;

    bar_bg_style = *lv_bar_get_style(barStart, LV_BAR_STYLE_BG);
    bar_indic_style = *lv_bar_get_style(barStart, LV_BAR_STYLE_INDIC);

    bar_bg_style.body.radius = 0;
    bar_indic_style.body.radius = 0;

    lv_bar_set_style(barStart, LV_BAR_STYLE_BG, &bar_bg_style);
    lv_bar_set_style(barStart, LV_BAR_STYLE_INDIC, &bar_indic_style);

    lv_obj_set_size(barStart, 150, 20);

    lv_bar_set_value(barStart, 0, LV_ANIM_OFF);
    lv_bar_set_anim_time(barStart, 5000);
    lv_bar_set_value(barStart, 100, LV_ANIM_ON);
    lv_obj_align(barStart, NULL, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_auto_realign(barStart, true);
}

static void Task_PageJump(lv_task_t *task)
{
    Page_ChangeTo(PAGE_HOME);
}

/*
 ********************************************************************************************************
 *@func       Task_Create
 *@brief      创建任务/定时器
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Task_Create(void)
{
    taskPageJump = lv_task_create(Task_PageJump, 7000, LV_TASK_PRIO_MID, NULL);
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

    /*创建任务*/
    Task_Create();
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
    lv_task_del(taskPageJump);

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