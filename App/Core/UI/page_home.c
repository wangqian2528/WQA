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

/*日期信息*/
static lv_obj_t *lableDate;

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

// /*通道切换按钮*/
// static lv_obj_t *imgbtnChLeft;
// static lv_obj_t *imgbtnChRight;

// /*通道号显示*/
// static lv_obj_t *labelChannel;

/*测量值显示*/
static lv_obj_t *labCalValue;
static lv_obj_t *labCalUnit;

/*温度显示*/
static lv_obj_t *labelTempera;
static lv_obj_t *imgTempera;

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
        App_Printf("信息\r\n");
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
        Page_ChangeTo(PAGE_SET);
    }
    // else if (obj == imgbtnChLeft)
    // {
    //     App_Printf("左\r\n");
    // }
    // else if (obj == imgbtnChRight)
    // {
    //     App_Printf("右\r\n");
    // }
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
    static uint8_t toggle_flg = 0;
    static RTC_DateTypeDef GetDate;
    static RTC_TimeTypeDef GetTime;
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);
    if (toggle_flg)
        lv_label_set_text_fmt(lableTime, "%02d:%02d", GetTime.Hours, GetTime.Minutes);
    else
        lv_label_set_text_fmt(lableTime, "%02d %02d", GetTime.Hours, GetTime.Minutes);
    lv_label_set_text_fmt(lableDate, "%4d/%02d/%02d", 2000 + GetDate.Year, GetDate.Month, GetDate.Date);
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
    contTitle = lv_obj_create(appWindow);
    lv_obj_set_size(contTitle, 320, 40);
    lv_obj_clear_flag(contTitle, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t title_cont_style;
    lv_style_init(&title_cont_style);
    lv_style_set_bg_color(&title_cont_style, lv_color_black());
    lv_style_set_bg_grad_color(&title_cont_style, lv_color_make(0xA9, 0xA9, 0xA9));
    lv_style_set_bg_grad_dir(&title_cont_style, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&title_cont_style, 0);

    lv_style_set_radius(&title_cont_style, 0);
    lv_style_set_pad_all(&title_cont_style, 0);
    lv_obj_add_style(contTitle, &title_cont_style, 0);
    lv_obj_align(contTitle, LV_ALIGN_TOP_MID, 0, 0);

    //时间显示
    LV_FONT_DECLARE(SYHT_MED_16);
    lableTime = lv_label_create(contTitle);

    static lv_style_t lable_time_style;
    lv_style_init(&lable_time_style);
    lv_style_set_text_color(&lable_time_style, lv_color_white());
    lv_style_set_text_font(&lable_time_style, &SYHT_MED_16);
    lv_style_set_pad_all(&lable_time_style, 0);

    lv_obj_add_style(lableTime, &lable_time_style, 0);
    lv_obj_align(lableTime, LV_ALIGN_BOTTOM_RIGHT, -3, 5);

    //日期显示
    LV_FONT_DECLARE(SYHT_MED_16);
    lableDate = lv_label_create(contTitle);
    lv_obj_add_style(lableDate, &lable_time_style, 0);
    lv_obj_align(lableDate, LV_ALIGN_TOP_RIGHT, -3, 5);
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
    //显示曲线图标
    LV_IMG_DECLARE(LChartW);
    LV_IMG_DECLARE(LChartB);
    imgbtnLChart = lv_imgbtn_create(appWindow);
    lv_imgbtn_set_src(imgbtnLChart, LV_IMGBTN_STATE_RELEASED, NULL, &LChartB, NULL);
    lv_imgbtn_set_src(imgbtnLChart, LV_IMGBTN_STATE_PRESSED, NULL, &LChartW, NULL);
    lv_obj_set_size(imgbtnLChart, 32, 32);
    lv_obj_align(imgbtnLChart, LV_ALIGN_LEFT_MID, 0, -60);
    lv_obj_add_event_cb(imgbtnLChart, btn_event_cb, LV_EVENT_CLICKED, NULL);

    //菜单按钮
    LV_FONT_DECLARE(SYHT_MED_16);

    static lv_style_t btn_style_rel, btn_style_pr;
    lv_style_init(&btn_style_rel);
    lv_style_set_text_font(&btn_style_rel, &SYHT_MED_16);
    lv_style_set_text_color(&btn_style_rel, lv_color_black());
    lv_style_set_bg_color(&btn_style_rel, lv_color_white());
    lv_style_set_border_width(&btn_style_rel, 2);
    lv_style_set_border_color(&btn_style_rel, lv_color_black());

    lv_style_init(&btn_style_pr);
    lv_style_set_text_font(&btn_style_pr, &SYHT_MED_16);
    lv_style_set_text_color(&btn_style_pr, lv_color_white());
    lv_style_set_bg_color(&btn_style_pr, lv_color_black());
    lv_style_set_border_width(&btn_style_pr, 2);
    lv_style_set_border_color(&btn_style_pr, lv_color_black());

    btnSet = lv_btn_create(appWindow);
    lv_obj_set_size(btnSet, 75, 40);
    lv_obj_add_style(btnSet, &btn_style_rel, 0);
    lv_obj_add_style(btnSet, &btn_style_pr, LV_STATE_PRESSED);
    labelSet = lv_label_create(btnSet);
    lv_label_set_text(labelSet, "设置");
    lv_obj_align(labelSet, LV_ALIGN_CENTER, 0, 4);
    lv_obj_align(btnSet, LV_ALIGN_BOTTOM_LEFT, 3, -3);

    btnCal = lv_btn_create(appWindow);
    lv_obj_set_size(btnCal, 75, 40);
    lv_obj_add_style(btnCal, &btn_style_rel, 0);
    lv_obj_add_style(btnCal, &btn_style_pr, LV_STATE_PRESSED);
    labelCal = lv_label_create(btnCal);
    lv_label_set_text(labelCal, "校准");
    lv_obj_align(labelCal, LV_ALIGN_CENTER, 0, 4);
    lv_obj_align_to(btnCal, btnSet, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnMro = lv_btn_create(appWindow);
    lv_obj_set_size(btnMro, 75, 40);
    lv_obj_add_style(btnMro, &btn_style_rel, 0);
    lv_obj_add_style(btnMro, &btn_style_pr, LV_STATE_PRESSED);
    labelMro = lv_label_create(btnMro);
    lv_label_set_text(labelMro, "维护");
    lv_obj_align(labelMro, LV_ALIGN_CENTER, 0, 4);
    lv_obj_align_to(btnMro, btnCal, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnInfo = lv_btn_create(appWindow);
    lv_obj_set_size(btnInfo, 75, 40);
    lv_obj_add_style(btnInfo, &btn_style_rel, 0);
    lv_obj_add_style(btnInfo, &btn_style_pr, LV_STATE_PRESSED);
    labelInfo = lv_label_create(btnInfo);
    lv_label_set_text(labelInfo, "信息");
    lv_obj_align(labelInfo, LV_ALIGN_CENTER, 0, 4);
    lv_obj_align_to(btnInfo, btnMro, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    lv_obj_add_event_cb(btnSet, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnCal, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnMro, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnInfo, btn_event_cb, LV_EVENT_CLICKED, NULL);

    // //通道号显示
    // LV_FONT_DECLARE(SYHT_BOLD_20);
    // labelChannel = lv_label_create(appWindow, NULL);
    // static lv_style_t label_channel_style;
    // label_channel_style = *lv_label_get_style(labelChannel, LV_LABEL_STYLE_MAIN);
    // label_channel_style.text.font = &SYHT_BOLD_20;
    // label_channel_style.text.color = LV_COLOR_BLACK;
    // lv_label_set_style(labelChannel, LV_LABEL_STYLE_MAIN, &label_channel_style);
    // lv_obj_align(labelChannel, NULL, LV_ALIGN_CENTER, 0, -50);
    // lv_obj_set_auto_realign(labelChannel, true);
    // lv_label_set_text(labelChannel, "Channel_1");

    // //通道切换图标
    // LV_IMG_DECLARE(LeftW);
    // LV_IMG_DECLARE(LeftB);
    // LV_IMG_DECLARE(RightW);
    // LV_IMG_DECLARE(RightB);
    // imgbtnChLeft = lv_imgbtn_create(appWindow, NULL);
    // lv_obj_set_size(imgbtnChLeft, 32, 32);
    // lv_imgbtn_set_src(imgbtnChLeft, LV_BTN_STATE_REL, &LeftB);
    // lv_imgbtn_set_src(imgbtnChLeft, LV_BTN_STATE_PR, &LeftW);
    // imgbtnChRight = lv_imgbtn_create(appWindow, NULL);
    // lv_obj_set_size(imgbtnChRight, 32, 32);
    // lv_imgbtn_set_src(imgbtnChRight, LV_BTN_STATE_REL, &RightB);
    // lv_imgbtn_set_src(imgbtnChRight, LV_BTN_STATE_PR, &RightW);
    // lv_obj_set_event_cb(imgbtnChLeft, btn_event_cb);
    // lv_obj_set_event_cb(imgbtnChRight, btn_event_cb);

    // lv_obj_align(imgbtnChLeft, labelChannel, LV_ALIGN_OUT_LEFT_MID, -5, -5);
    // lv_obj_align(imgbtnChRight, labelChannel, LV_ALIGN_OUT_RIGHT_MID, 5, -5);
    // lv_obj_set_auto_realign(imgbtnChLeft, true);
    // lv_obj_set_auto_realign(imgbtnChRight, true);

    //测量值显示
    LV_FONT_DECLARE(SYHT_BOLD_40);
    labCalValue = lv_label_create(appWindow);
    lv_obj_set_style_text_font(labCalValue, &SYHT_BOLD_40, 0);
    lv_obj_set_style_text_color(labCalValue, lv_color_black(), 0);
    lv_obj_align(labCalValue, LV_ALIGN_CENTER, 0, -10);
    lv_label_set_text(labCalValue, "98.72");

    //测量单位显示
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labCalUnit = lv_label_create(appWindow);
    lv_obj_set_style_text_font(labCalUnit, &SYHT_BOLD_20, 0);
    lv_obj_set_style_text_color(labCalUnit, lv_color_black(), 0);
    lv_obj_align_to(labCalUnit, labCalValue, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, -12);
    lv_label_set_text(labCalUnit, "msC");

    //温度显示
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labelTempera = lv_label_create(appWindow);
    lv_obj_set_style_text_font(labelTempera, &SYHT_BOLD_20, 0);
    lv_obj_set_style_text_color(labelTempera, lv_color_black(), 0);
    lv_obj_align(labelTempera, LV_ALIGN_CENTER, 7, 40);
    lv_label_set_text(labelTempera, "30.1 ℃");

    //温度图标
    LV_IMG_DECLARE(IconTemp);
    imgTempera = lv_img_create(appWindow);
    lv_img_set_src(imgTempera, &IconTemp);
    lv_obj_align_to(imgTempera, labelTempera, LV_ALIGN_OUT_LEFT_MID, -8, -5);
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