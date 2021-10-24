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
    contTitle = lv_cont_create(appWindow, NULL);
    lv_obj_set_size(contTitle, 320, 40);
    static lv_style_t title_cont_style;
    title_cont_style = *lv_cont_get_style(contTitle, LV_LABEL_STYLE_MAIN);
    title_cont_style.body.main_color = LV_COLOR_BLACK;
    title_cont_style.body.grad_color = LV_COLOR_DARK_GRAY;
    title_cont_style.body.radius = 0;
    lv_cont_set_style(contTitle, LV_LABEL_STYLE_MAIN, &title_cont_style);
    lv_obj_align(contTitle, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    //时间显示
    LV_FONT_DECLARE(SYHT_MED_16);
    lableTime = lv_label_create(contTitle, NULL);
    static lv_style_t lable_time_style;
    lable_time_style = *lv_label_get_style(lableTime, LV_LABEL_STYLE_MAIN);
    lable_time_style.text.font = &SYHT_MED_16;
    lable_time_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(lableTime, LV_LABEL_STYLE_MAIN, &lable_time_style);
    lv_obj_align(lableTime, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -3, 7);
    lv_obj_set_auto_realign(lableTime, true);

    //日期显示
    LV_FONT_DECLARE(SYHT_MED_16);
    lableDate = lv_label_create(contTitle, lableTime);
    lv_obj_align(lableDate, NULL, LV_ALIGN_IN_TOP_RIGHT, -3, 3);
    lv_obj_set_auto_realign(lableDate, true);
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
    imgbtnLChart = lv_imgbtn_create(appWindow, NULL);
    lv_imgbtn_set_src(imgbtnLChart, LV_BTN_STATE_REL, &LChartB);
    lv_imgbtn_set_src(imgbtnLChart, LV_BTN_STATE_PR, &LChartW);
    lv_obj_set_size(imgbtnLChart, 32, 32);
    lv_obj_align(imgbtnLChart, NULL, LV_ALIGN_IN_LEFT_MID, 0, -60);
    lv_obj_set_event_cb(imgbtnLChart, btn_event_cb);

    //菜单按钮
    LV_FONT_DECLARE(SYHT_MED_16);

    btnSet = lv_btn_create(appWindow, NULL);
    lv_obj_set_size(btnSet, 75, 40);
    static lv_style_t btn_set_style_rel, btn_set_style_pr;
    btn_set_style_rel = *lv_btn_get_style(btnSet, LV_BTN_STYLE_REL);
    btn_set_style_pr = *lv_btn_get_style(btnSet, LV_BTN_STYLE_PR);
    btn_set_style_rel.text.font = &SYHT_MED_16;
    btn_set_style_pr.text.font = &SYHT_MED_16;
    lv_btn_set_style(btnSet, LV_BTN_STYLE_REL, &btn_set_style_rel);
    lv_btn_set_style(btnSet, LV_BTN_STYLE_PR, &btn_set_style_pr);
    lv_btn_set_layout(btnSet, LV_LAYOUT_OFF);
    labelSet = lv_label_create(btnSet, NULL);
    lv_label_set_text(labelSet, "设置");
    lv_obj_align(btnSet, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 3, -2);
    lv_obj_align(labelSet, NULL, LV_ALIGN_CENTER, 0, 5);

    btnCal = lv_btn_create(appWindow, btnSet);
    labelCal = lv_label_create(btnCal, labelSet);
    lv_label_set_text(labelCal, "校准");
    lv_obj_align(btnCal, btnSet, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnMro = lv_btn_create(appWindow, btnSet);
    labelMro = lv_label_create(btnMro, labelSet);
    lv_label_set_text(labelMro, "维护");
    lv_obj_align(btnMro, btnCal, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    btnInfo = lv_btn_create(appWindow, btnSet);
    labelInfo = lv_label_create(btnInfo, labelSet);
    lv_label_set_text(labelInfo, "信息");
    lv_obj_align(btnInfo, btnMro, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);

    lv_obj_set_event_cb(btnSet, btn_event_cb);
    lv_obj_set_event_cb(btnCal, btn_event_cb);
    lv_obj_set_event_cb(btnMro, btn_event_cb);
    lv_obj_set_event_cb(btnInfo, btn_event_cb);

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
    labCalValue = lv_label_create(appWindow, NULL);
    static lv_style_t label_cal_value;
    label_cal_value = *lv_label_get_style(labCalValue, LV_LABEL_STYLE_MAIN);
    label_cal_value.text.font = &SYHT_BOLD_40;
    label_cal_value.text.color = LV_COLOR_BLACK;
    lv_label_set_style(labCalValue, LV_LABEL_STYLE_MAIN, &label_cal_value);
    lv_obj_align(labCalValue, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(labCalValue, true);
    lv_label_set_text(labCalValue, "98.72");

    //测量单位显示
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labCalUnit = lv_label_create(appWindow, NULL);
    static lv_style_t label_cal_unit;
    label_cal_unit = *lv_label_get_style(labCalUnit, LV_LABEL_STYLE_MAIN);
    label_cal_unit.text.font = &SYHT_BOLD_20;
    label_cal_unit.text.color = LV_COLOR_BLACK;
    lv_label_set_style(labCalUnit, LV_LABEL_STYLE_MAIN, &label_cal_unit);
    lv_obj_align(labCalUnit, labCalValue, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, -12);
    lv_obj_set_auto_realign(labCalUnit, true);
    lv_label_set_text(labCalUnit, "msC");

    //温度显示
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labelTempera = lv_label_create(appWindow, NULL);
    static lv_style_t label_tempera_style;
    label_tempera_style = *lv_label_get_style(labelTempera, LV_LABEL_STYLE_MAIN);
    label_tempera_style.text.font = &SYHT_BOLD_20;
    label_tempera_style.text.color = LV_COLOR_BLACK;
    lv_label_set_style(labelTempera, LV_LABEL_STYLE_MAIN, &label_tempera_style);
    lv_obj_align(labelTempera, NULL, LV_ALIGN_CENTER, 7, 40);
    lv_obj_set_auto_realign(labelTempera, true);
    lv_label_set_text(labelTempera, "30.1 ℃");

    //温度图标
    LV_IMG_DECLARE(IconTemp);
    imgTempera = lv_img_create(appWindow, NULL);
    lv_img_set_src(imgTempera, &IconTemp);
    lv_obj_align(imgTempera, labelTempera, LV_ALIGN_OUT_LEFT_MID, -8, -5);
    lv_obj_set_auto_realign(imgTempera, true);
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
    /*关任务*/
    lv_task_del(taskTimeUpdate);

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