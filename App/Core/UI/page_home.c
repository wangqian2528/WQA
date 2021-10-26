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
static lv_obj_t *contBody;

static lv_obj_t *ledState;
static lv_obj_t *lableDateTime;
static lv_obj_t *labelWarning;

/*曲线按钮*/
static lv_obj_t *imgbtnLChart;

/*菜单按钮*/
static lv_obj_t *btnInfo;
static lv_obj_t *btnCal;
static lv_obj_t *btnMro;
static lv_obj_t *btnSet;

static lv_obj_t *labelInfo;
static lv_obj_t *labelCal;
static lv_obj_t *labelMro;
static lv_obj_t *labelSet;

/*测量值显示*/
static lv_obj_t *labCalValue;
static lv_obj_t *labCalUnit;

/*温度显示*/
static lv_obj_t *labelTempera;
static lv_obj_t *imgTempera;
static lv_obj_t *imgTempUnit;

/*时间刷新任务*/
static lv_timer_t *tmrUpdate;

/*
 ********************************************************************************************************
 *@func       btn_event_cb
 *@brief      按键事件回调
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void btn_event_cb(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_target(event);
    if (obj == btnInfo)
    {
        Page_ChangeTo(PAGE_INFO);
    }
    else if (obj == btnCal)
    {
        App_Printf("校准\r\n");
    }
    else if (obj == btnMro)
    {
        App_Printf("维护\r\n");
    }
    else if (obj == btnSet)
    {
        App_Printf("设置\r\n");
    }
    else if (obj == imgbtnLChart)
    {
        App_Printf("曲线\r\n");
    }
}

/*
 ********************************************************************************************************
 *@func       TimerCB_TimeUpdate
 *@brief      时间刷新回调
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TimerCB_TimeUpdate(lv_timer_t *tmr)
{
    static RTC_DateTypeDef GetDate;
    static RTC_TimeTypeDef GetTime;
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);
    lv_label_set_text_fmt(lableDateTime, "%4d/%02d/%02d %02d:%02d",
                          2000 + GetDate.Year,
                          GetDate.Month,
                          GetDate.Date,
                          GetTime.Hours,
                          GetTime.Minutes);

    //LED翻转
    lv_led_toggle(ledState);
}

/*
 ********************************************************************************************************
 *@func       Layout_Creat
 *@brief      创建布局
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Layout_Creat(void)
{
    contTitle = lv_obj_create(appWindow);
    contBody = lv_obj_create(appWindow);

    lv_obj_set_style_pad_row(appWindow, 5, 0);

    static lv_coord_t grid_win_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_win_row_dsc[] = {30, 195, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(appWindow, grid_win_col_dsc, grid_win_row_dsc);

    lv_obj_set_grid_cell(contTitle, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(contBody, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
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
    lv_obj_clear_flag(contTitle, LV_OBJ_FLAG_SCROLLABLE);

    //日期时间显示
    lableDateTime = lv_label_create(contTitle);
    lv_obj_set_style_text_opa(lableDateTime, LV_OPA_60, 0);
    lv_obj_align(lableDateTime, LV_ALIGN_RIGHT_MID, 7, 0);

    //LED
    ledState = lv_led_create(contTitle);
    lv_obj_set_size(ledState, 14, 14);
    lv_led_set_brightness(ledState, 60);
    lv_led_set_color(ledState, lv_palette_main(LV_PALETTE_RED));
    lv_obj_align(ledState, LV_ALIGN_LEFT_MID, -5, 0);

    //告警
    labelWarning = lv_label_create(contTitle);
    lv_obj_set_style_text_opa(labelWarning, LV_OPA_60, 0);
    lv_label_set_text(labelWarning, LV_SYMBOL_WARNING);
    lv_obj_align(labelWarning, LV_ALIGN_LEFT_MID, 23, 0);
}

/*
 ********************************************************************************************************
 *@func       Body_Creat
 *@brief      创建主显示区控件
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void Body_Creat(void)
{
    lv_obj_set_style_pad_all(contBody, 5, 0);

    //菜单按钮
    btnInfo = lv_btn_create(contBody);
    lv_obj_set_style_text_font(btnInfo, &my_font_14, 0);
    lv_obj_set_style_text_font(btnInfo, &my_font_14, LV_STATE_PRESSED);
    lv_obj_set_size(btnInfo, 70, 40);
    labelInfo = lv_label_create(btnInfo);
    lv_label_set_text(labelInfo, "信息");
    lv_obj_align(labelInfo, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btnInfo, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    btnCal = lv_btn_create(contBody);
    lv_obj_set_style_text_font(btnCal, &my_font_14, 0);
    lv_obj_set_style_text_font(btnCal, &my_font_14, LV_STATE_PRESSED);
    lv_obj_set_size(btnCal, 70, 40);
    labelCal = lv_label_create(btnCal);
    lv_label_set_text(labelCal, "校准");
    lv_obj_align(labelCal, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(btnCal, btnInfo, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnMro = lv_btn_create(contBody);
    lv_obj_set_style_text_font(btnMro, &my_font_14, 0);
    lv_obj_set_style_text_font(btnMro, &my_font_14, LV_STATE_PRESSED);
    lv_obj_set_size(btnMro, 70, 40);
    labelMro = lv_label_create(btnMro);
    lv_label_set_text(labelMro, "维护");
    lv_obj_align(labelMro, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(btnMro, btnCal, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnSet = lv_btn_create(contBody);
    lv_obj_set_style_text_font(btnSet, &my_font_14, 0);
    lv_obj_set_style_text_font(btnSet, &my_font_14, LV_STATE_PRESSED);
    lv_obj_set_size(btnSet, 70, 40);
    labelSet = lv_label_create(btnSet);
    lv_label_set_text(labelSet, "设置");
    lv_obj_align(labelSet, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(btnSet, btnMro, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    lv_obj_add_event_cb(btnInfo, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnSet, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnCal, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnMro, btn_event_cb, LV_EVENT_CLICKED, NULL);

    //测量值显示
    labCalValue = lv_label_create(contBody);
    lv_obj_set_style_text_font(labCalValue, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_opa(labCalValue, LV_OPA_70, 0);
    lv_obj_align(labCalValue, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_text(labCalValue, "98.72");

    //测量单位显示
    labCalUnit = lv_label_create(contBody);
    lv_obj_set_style_text_font(labCalUnit, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_opa(labCalUnit, LV_OPA_70, 0);
    lv_obj_align_to(labCalUnit, labCalValue, LV_ALIGN_OUT_RIGHT_BOTTOM, 7, -4);
    lv_label_set_text(labCalUnit, "uS/cm");

    //温度显示
    labelTempera = lv_label_create(contBody);
    lv_obj_set_style_text_font(labelTempera, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_opa(labelTempera, LV_OPA_70, 0);
    lv_obj_align(labelTempera, LV_ALIGN_CENTER, 7, 7);
    lv_label_set_text(labelTempera, "30.1");

    //温度图标
    LV_IMG_DECLARE(IconTemp);
    LV_IMG_DECLARE(IconCels);
    imgTempera = lv_img_create(contBody);
    lv_img_set_src(imgTempera, &IconTemp);
    lv_obj_align_to(imgTempera, labelTempera, LV_ALIGN_OUT_LEFT_MID, -8, 0);

    imgTempUnit = lv_img_create(contBody);
    lv_img_set_src(imgTempUnit, &IconCels);
    lv_obj_set_style_img_opa(imgTempUnit, LV_OPA_60, 0);
    lv_obj_align_to(imgTempUnit, labelTempera, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
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
    tmrUpdate = lv_timer_create(TimerCB_TimeUpdate, 2000, NULL);
    TimerCB_TimeUpdate(tmrUpdate);
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
    Layout_Creat();
    ContTitle_Creat();
    Body_Creat();

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
    /*删除任务*/
    lv_timer_del(tmrUpdate);

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