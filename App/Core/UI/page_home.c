/******************************************************************************
 * @brief    主页面
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

/*标题栏容器*/
static lv_obj_t *contTitle;

/*时间信息*/
static lv_obj_t *lableTime;

/*时间修改按钮*/
static lv_obj_t *imgBtnTime;

/*时间刷新任务*/
static lv_task_t *taskTimeUpdate;

/*
 ********************************************************************************************************
 *@func       btn_event_cb
 *@brief      按键事件回调
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void btn_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED)
    {
    }
}

/*
 ********************************************************************************************************
 *@func       Task_TimeUpdate
 *@brief      时间刷新回调
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Task_TimeUpdate(lv_task_t *task)
{
    static uint8_t toggle_flg = 0;
    static RTC_DateTypeDef GetData;
    static RTC_TimeTypeDef GetTime;
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
    if (toggle_flg)
        lv_label_set_text_fmt(lableTime, "%02d:%02d", GetTime.Hours, GetTime.Minutes);
    else
        lv_label_set_text_fmt(lableTime, "%02d %02d", GetTime.Hours, GetTime.Minutes);
    toggle_flg = !toggle_flg;
}

/*
 ********************************************************************************************************
 *@func       ContTitle_Creat
 *@brief      创建标题栏
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void ContTitle_Creat(void)
{
    //标题栏
    contTitle = lv_cont_create(appWindow, NULL);
    lv_obj_set_size(contTitle, 320, 40);
    static lv_style_t title_cont_style;
    title_cont_style = *lv_cont_get_style(contTitle, LV_LABEL_STYLE_MAIN);
    title_cont_style.body.main_color = LV_COLOR_BLACK;
    title_cont_style.body.grad_color = LV_COLOR_DARK_GRAY;
    title_cont_style.body.radius = 0;
    lv_cont_set_style(contTitle, LV_LABEL_STYLE_MAIN, &title_cont_style);
    lv_obj_align(contTitle, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_auto_realign(contTitle, true);

    //时间调整按钮
    LV_IMG_DECLARE(IconTimeW);
    LV_IMG_DECLARE(IconTimeB);
    imgBtnTime = lv_imgbtn_create(contTitle, NULL);
    lv_obj_set_size(imgBtnTime, 32, 32);
    static lv_style_t btn_time_on, btn_time_off;
    btn_time_on = *lv_btn_get_style(imgBtnTime, LV_BTN_STYLE_PR);
    btn_time_off = *lv_btn_get_style(imgBtnTime, LV_BTN_STYLE_REL);
    btn_time_off.body.opa = LV_OPA_TRANSP;
    btn_time_on.body.opa = LV_OPA_TRANSP;
    lv_imgbtn_set_src(imgBtnTime, LV_BTN_STATE_PR, &IconTimeB);
    lv_imgbtn_set_src(imgBtnTime, LV_BTN_STATE_REL, &IconTimeW);
    lv_obj_align(imgBtnTime, NULL, LV_ALIGN_IN_RIGHT_MID, -55, 0);
    lv_obj_set_auto_realign(imgBtnTime, true);
    lv_obj_set_event_cb(imgBtnTime, btn_event_cb);

    //时间显示
    LV_FONT_DECLARE(SYHT_MED_16);
    lableTime = lv_label_create(contTitle, NULL);
    static lv_style_t lable_time_style;
    lable_time_style = *lv_label_get_style(lableTime, LV_LABEL_STYLE_MAIN);
    lable_time_style.text.font = &SYHT_MED_16;
    lable_time_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(lableTime, LV_LABEL_STYLE_MAIN, &lable_time_style);
    lv_obj_align(lableTime, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 3);
    lv_obj_set_auto_realign(lableTime, true);
}

/*
 ********************************************************************************************************
 *@func       Task_Create
 *@brief      创建任务
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Task_Create(void)
{
    taskTimeUpdate = lv_task_create(Task_TimeUpdate, 2000, LV_TASK_PRIO_MID, NULL);
    Task_TimeUpdate(taskTimeUpdate);
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
    ContTitle_Creat();

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

    /*删除此页面上的子控件*/
    lv_obj_clean(appWindow);
}

/*
 ********************************************************************************************************
 *@brief      页面控制器
 ********************************************************************************************************
 */
Page_TypeDef page_Home = {
    .pageID = PAGE_HOME,
    .exitCallback = Exit,
    .setupCallback = Setup,
    .next = NULL,
};

/*
 ********************************************************************************************************
 *@func       PageRegister_Home
 *@brief      页面注册
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
void PageRegister_Home(Page_Type pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    /*注册至页面调度器*/
    Page_Register(&page_Home);
}