/******************************************************************************
 * @brief    维护页面
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

typedef enum
{
    SET_MEAS = 0,
    SET_RELAY,
    SET_ANALOG,
    SET_DIGITAL,
    SET_USB,
    SET_SYSTEM,
    SET_PASSWD,
    SET_RESET,
    SET_MAX,
} SET_NAME_E;

const char *set_name[] = {
    "测量设置",
    "继电器设置",
    "模拟输出设置",
    "数字输出设置",
    "USB输出设置",
    "系统设置",
    "密码管理",
    "重置",
};

/*此页面窗口*/
static lv_obj_t *appWindow;

/*标题栏容器*/
static lv_obj_t *contTitle;
static lv_obj_t *contBody;
static lv_obj_t *btnBack;

static lv_obj_t *btnLeft;
static lv_obj_t *btnRight;

static lv_obj_t *labelTabName;
static lv_obj_t *labelNextTabName;
static lv_obj_t *labelLastTabName;

static SET_NAME_E tab_index;

/*子窗口*/
static lv_obj_t *tabMeas;
static lv_obj_t *tabRelay;
static lv_obj_t *tabAnalog;
static lv_obj_t *tabDigital;
static lv_obj_t *tabUSB;
static lv_obj_t *tabSystem;
static lv_obj_t *tabPasswd;
static lv_obj_t *tabReset;

//Measure
static lv_obj_t *dropdown_meas_para;
static lv_obj_t *dropdown_meas_sensor;
static lv_obj_t *dropdown_meas_temp;
static lv_obj_t *dropdown_meas_mod;
static lv_obj_t *text_meas_time;
static lv_obj_t *dropdown_meas_time;
static lv_obj_t *text_meas_filter;
static lv_obj_t *text_meas_tempVal;
static lv_obj_t *dropdown_meas_tempUnit;
static lv_obj_t *text_meas_tempComp;
static lv_obj_t *dropdown_meas_gear;
static lv_obj_t *dropdown_meas_compmod;
static lv_obj_t *text_meas_compVal;
static lv_obj_t *dropdown_meas_freq;

//Relay
static lv_obj_t *dropdown_relay_1;
static lv_obj_t *dropdown_relay_2;
static lv_obj_t *dropdown_relay_3;
static lv_obj_t *dropdown_relay_4;
static lv_obj_t *dropdown_relay_5;
static lv_obj_t *dropdown_relay_6;
static lv_obj_t *dropdown_relay_7;
static lv_obj_t *dropdown_relay_8;
static lv_obj_t *dropdown_relay_9;
static lv_obj_t *dropdown_relay_10;

static lv_obj_t *text_relay_4;
static lv_obj_t *text_relay_5;
static lv_obj_t *text_relay_6;
static lv_obj_t *text_relay_7;
static lv_obj_t *text_relay_8;
static lv_obj_t *text_relay_9;
static lv_obj_t *text_relay_10;

//Analog
static lv_obj_t *dropdown_analog_1;
static lv_obj_t *dropdown_analog_2;
static lv_obj_t *dropdown_analog_3;
static lv_obj_t *dropdown_analog_4;
static lv_obj_t *dropdown_analog_5;

static lv_obj_t *text_analog_4;
static lv_obj_t *text_analog_5;

//Digial
static lv_obj_t *text_digital_1;
static lv_obj_t *dropdown_digital_2;
static lv_obj_t *dropdown_digital_3;
static lv_obj_t *dropdown_digital_4;
static lv_obj_t *dropdown_digital_5;

//USB
static lv_obj_t *dropdown_usb_1;
static lv_obj_t *text_usb_2;

//SYSTEM
static lv_obj_t *dropdown_sys_1;
static lv_obj_t *text_sys_2;
static lv_obj_t *text_sys_3;
static lv_obj_t *dropdown_sys_4;
static lv_obj_t *text_sys_5;
static lv_obj_t *slider_sys_6;
static lv_obj_t *text_sys_7;

/*
 ********************************************************************************************************
 *@func       tab_name_refresh
 *@brief      重绘tab name
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void tab_name_refresh(void)
{
    if (tab_index == SET_MEAS)
    {
        lv_label_set_text(labelNextTabName, set_name[SET_RELAY]);
        lv_label_set_text(labelLastTabName, set_name[SET_RESET]);
    }
    else if (tab_index == SET_RESET)
    {
        lv_label_set_text(labelNextTabName, set_name[SET_MEAS]);
        lv_label_set_text(labelLastTabName, set_name[SET_PASSWD]);
    }
    else
    {
        lv_label_set_text(labelNextTabName, set_name[tab_index + 1]);
        lv_label_set_text(labelLastTabName, set_name[tab_index - 1]);
    }
    lv_label_set_text(labelTabName, set_name[tab_index]);
    lv_tabview_set_act(contBody, tab_index, LV_ANIM_ON);
}

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
    if (obj == btnBack)
    {
        Page_ChangeTo(PAGE_HOME);
    }
    else if (obj == btnLeft)
    {
        if (tab_index > SET_MEAS)
            tab_index--;
        else
            tab_index = SET_RESET;
        tab_name_refresh();
    }
    else if (obj == btnRight)
    {
        if (tab_index < SET_RESET)
            tab_index++;
        else
            tab_index = SET_MEAS;
        tab_name_refresh();
    }
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
    contBody = lv_tabview_create(appWindow, LV_DIR_TOP, 0);
    lv_obj_clear_flag(lv_tabview_get_content(contBody), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(contBody, 2, 0);
    lv_obj_set_style_border_color(contBody, lv_color_make(0xdb, 0xdb, 0xdb), 0);
    lv_obj_set_style_radius(contBody, 7, 0);

    lv_obj_set_style_pad_row(appWindow, 5, 0);

    static lv_coord_t grid_win_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_win_row_dsc[] = {30, 195, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(appWindow, grid_win_col_dsc, grid_win_row_dsc);

    lv_obj_set_grid_cell(contTitle, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(contBody, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

    //创建返回按钮
    btnBack = lv_btn_create(appWindow);
    lv_obj_set_size(btnBack, 26, 26);
    lv_obj_set_style_radius(btnBack, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_radius(btnBack, LV_RADIUS_CIRCLE, LV_STATE_PRESSED);
    lv_obj_set_style_text_color(btnBack, lv_color_white(), 0);
    lv_obj_set_style_text_color(btnBack, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_text_font(btnBack, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_font(btnBack, &lv_font_montserrat_20, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(btnBack, 0, 0);
    lv_obj_set_style_shadow_opa(btnBack, 0, LV_STATE_PRESSED);
    static lv_obj_t *label_back;
    label_back = lv_label_create(btnBack);
    lv_label_set_text(label_back, LV_SYMBOL_NEW_LINE);
    lv_obj_center(label_back);
    lv_obj_set_grid_cell(btnBack, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_END, 1, 1);
    lv_obj_add_event_cb(btnBack, btn_event_cb, LV_EVENT_CLICKED, NULL);
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
    lv_obj_set_style_pad_hor(contTitle, 3, 0);

    //左右切换按键
    static lv_style_t btn_style;
    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, LV_RADIUS_CIRCLE);
    lv_style_set_text_color(&btn_style, lv_color_white());
    lv_style_set_text_font(&btn_style, &lv_font_montserrat_14);
    lv_style_set_shadow_opa(&btn_style, 0);

    btnLeft = lv_btn_create(contTitle);
    btnRight = lv_btn_create(contTitle);
    lv_obj_set_size(btnLeft, 20, 20);
    lv_obj_set_size(btnRight, 20, 20);
    lv_obj_add_style(btnLeft, &btn_style, 0);
    lv_obj_add_style(btnLeft, &btn_style, LV_STATE_PRESSED);
    lv_obj_add_style(btnRight, &btn_style, 0);
    lv_obj_add_style(btnRight, &btn_style, LV_STATE_PRESSED);

    lv_obj_align(btnLeft, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(btnRight, LV_ALIGN_RIGHT_MID, 0, 0);

    static lv_obj_t *label_left;
    static lv_obj_t *label_right;
    label_left = lv_label_create(btnLeft);
    label_right = lv_label_create(btnRight);
    lv_obj_center(label_left);
    lv_obj_center(label_right);
    lv_label_set_text(label_left, LV_SYMBOL_LEFT);
    lv_label_set_text(label_right, LV_SYMBOL_RIGHT);

    lv_obj_add_event_cb(btnLeft, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnRight, btn_event_cb, LV_EVENT_CLICKED, NULL);

    //TAB NAME
    labelTabName = lv_label_create(contTitle);
    lv_obj_set_style_text_font(labelTabName, &my_font_14, 0);
    // lv_obj_set_style_text_opa(labelTabName, LV_OPA_90, 0);
    lv_obj_center(labelTabName);

    labelLastTabName = lv_label_create(contTitle);
    lv_label_set_long_mode(labelLastTabName, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_speed(labelLastTabName, 15, 0);
    lv_obj_set_width(labelLastTabName, 60);
    lv_obj_set_style_text_font(labelLastTabName, &my_font_14, 0);
    lv_obj_set_style_text_opa(labelLastTabName, LV_OPA_50, 0);
    lv_obj_align(labelLastTabName, LV_ALIGN_LEFT_MID, 30, 0);

    labelNextTabName = lv_label_create(contTitle);
    lv_label_set_long_mode(labelNextTabName, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_speed(labelNextTabName, 15, 0);
    lv_obj_set_width(labelNextTabName, 60);
    lv_obj_set_style_text_font(labelNextTabName, &my_font_14, 0);
    lv_obj_set_style_text_opa(labelNextTabName, LV_OPA_50, 0);
    lv_obj_align(labelNextTabName, LV_ALIGN_RIGHT_MID, -30, 0);
}

/*
 ********************************************************************************************************
 *@func       TabMeas_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabMeas_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "测量参数");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "传感器");

    lv_obj_t *label3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label3, &my_font_14, 0);
    lv_label_set_text(label3, "温度源");

    lv_obj_t *label4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label4, &my_font_14, 0);
    lv_label_set_text(label4, "测量工作模式");

    lv_obj_t *label5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label5, &my_font_14, 0);
    lv_label_set_text(label5, "定时测量时间");

    lv_obj_t *label6 = lv_label_create(parent);
    lv_obj_set_style_text_font(label6, &my_font_14, 0);
    lv_label_set_text(label6, "滤波时间");

    lv_obj_t *label7 = lv_label_create(parent);
    lv_obj_set_style_text_font(label7, &my_font_14, 0);
    lv_label_set_text(label7, "MTC温度值");

    lv_obj_t *label8 = lv_label_create(parent);
    lv_obj_set_style_text_font(label8, &my_font_14, 0);
    lv_label_set_text(label8, "温度补偿值");

    lv_obj_t *label9 = lv_label_create(parent);
    lv_obj_set_style_text_font(label9, &my_font_14, 0);
    lv_label_set_text(label9, "档位选择");

    lv_obj_t *label10 = lv_label_create(parent);
    lv_obj_set_style_text_font(label10, &my_font_14, 0);
    lv_label_set_text(label10, "温度补偿模式");

    lv_obj_t *label11 = lv_label_create(parent);
    lv_obj_set_style_text_font(label11, &my_font_14, 0);
    lv_label_set_text(label11, "线性补偿值");

    lv_obj_t *label12 = lv_label_create(parent);
    lv_obj_set_style_text_font(label12, &my_font_14, 0);
    lv_label_set_text(label12, "市电频率");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label5, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(label6, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(label7, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(label8, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 7, 1);
    lv_obj_set_grid_cell(label9, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);
    lv_obj_set_grid_cell(label10, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 9, 1);
    lv_obj_set_grid_cell(label11, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 10, 1);
    lv_obj_set_grid_cell(label12, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 11, 1);

    dropdown_meas_para = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_para, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    dropdown_meas_sensor = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_sensor, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_meas_temp = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_temp, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    dropdown_meas_mod = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_mod, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_meas_time = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_meas_time, true);
    lv_textarea_set_align(text_meas_time, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_meas_time, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    dropdown_meas_time = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_time, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    text_meas_filter = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_meas_filter, true);
    lv_textarea_set_align(text_meas_filter, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_meas_filter, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_CENTER, 5, 1);

    text_meas_tempVal = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_meas_tempVal, true);
    lv_textarea_set_align(text_meas_tempVal, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_meas_tempVal, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    dropdown_meas_tempUnit = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_tempUnit, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    text_meas_tempComp = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_meas_tempComp, true);
    lv_textarea_set_align(text_meas_tempComp, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_meas_tempComp, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_CENTER, 7, 1);

    dropdown_meas_gear = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_gear, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 8, 1);

    dropdown_meas_compmod = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_compmod, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 9, 1);

    text_meas_compVal = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_meas_compVal, true);
    lv_textarea_set_align(text_meas_compVal, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_meas_compVal, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_CENTER, 10, 1);

    dropdown_meas_freq = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_meas_freq, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 11, 1);
}

/*
 ********************************************************************************************************
 *@func       TabRelay_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabRelay_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "设定点");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "参数");

    lv_obj_t *label3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label3, &my_font_14, 0);
    lv_label_set_text(label3, "类型");

    lv_obj_t *label4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label4, &my_font_14, 0);
    lv_label_set_text(label4, "高点/低点");

    lv_obj_t *label5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label5, &my_font_14, 0);
    lv_label_set_text(label5, "回差");

    lv_obj_t *label6 = lv_label_create(parent);
    lv_obj_set_style_text_font(label6, &my_font_14, 0);
    lv_label_set_text(label6, "界内H/界外H");

    lv_obj_t *label7 = lv_label_create(parent);
    lv_obj_set_style_text_font(label7, &my_font_14, 0);
    lv_label_set_text(label7, "界内L/界外L");

    lv_obj_t *label8 = lv_label_create(parent);
    lv_obj_set_style_text_font(label8, &my_font_14, 0);
    lv_label_set_text(label8, "间隔时间");

    lv_obj_t *label9 = lv_label_create(parent);
    lv_obj_set_style_text_font(label9, &my_font_14, 0);
    lv_label_set_text(label9, "清洗时间");

    lv_obj_t *label10 = lv_label_create(parent);
    lv_obj_set_style_text_font(label10, &my_font_14, 0);
    lv_label_set_text(label10, "保持时间");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label5, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(label6, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(label7, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(label8, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 7, 1);
    lv_obj_set_grid_cell(label9, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);
    lv_obj_set_grid_cell(label10, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 9, 1);

    dropdown_relay_1 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_1, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    dropdown_relay_2 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_2, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_relay_3 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_3, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_relay_4 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_4, true);
    lv_textarea_set_align(text_relay_4, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_4, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    dropdown_relay_4 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_4, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_relay_5 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_5, true);
    lv_textarea_set_align(text_relay_5, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_5, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    dropdown_relay_5 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_5, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    text_relay_6 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_6, true);
    lv_textarea_set_align(text_relay_6, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_6, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    dropdown_relay_6 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_6, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    text_relay_7 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_7, true);
    lv_textarea_set_align(text_relay_7, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_7, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    dropdown_relay_7 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_7, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    text_relay_8 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_8, true);
    lv_textarea_set_align(text_relay_8, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_8, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 7, 1);

    dropdown_relay_8 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_8, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 7, 1);

    text_relay_9 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_9, true);
    lv_textarea_set_align(text_relay_9, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_9, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 8, 1);

    dropdown_relay_9 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_9, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 8, 1);

    text_relay_10 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_10, true);
    lv_textarea_set_align(text_relay_10, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_relay_10, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 9, 1);

    dropdown_relay_10 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_relay_10, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 9, 1);
}

/*
 ********************************************************************************************************
 *@func       TabAnalog_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabAnalog_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "设定点");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "测量参数");

    lv_obj_t *label3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label3, &my_font_14, 0);
    lv_label_set_text(label3, "范围");

    lv_obj_t *label4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label4, &my_font_14, 0);
    lv_label_set_text(label4, "最大值");

    lv_obj_t *label5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label5, &my_font_14, 0);
    lv_label_set_text(label5, "最小值");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label5, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    dropdown_analog_1 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_analog_1, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    dropdown_analog_2 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_analog_2, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_analog_3 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_analog_3, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_analog_4 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_4, true);
    lv_textarea_set_align(text_analog_4, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_analog_4, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    dropdown_analog_4 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_analog_4, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_analog_5 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_5, true);
    lv_textarea_set_align(text_analog_5, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_analog_5, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    dropdown_analog_5 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_analog_5, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
}

/*
 ********************************************************************************************************
 *@func       TabDigital_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabDigital_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "ID");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "模式");

    lv_obj_t *label3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label3, &my_font_14, 0);
    lv_label_set_text(label3, "波特率");

    lv_obj_t *label4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label4, &my_font_14, 0);
    lv_label_set_text(label4, "停止位");

    lv_obj_t *label5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label5, &my_font_14, 0);
    lv_label_set_text(label5, "校验位");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label5, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    text_digital_1 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_digital_1, true);
    lv_textarea_set_align(text_digital_1, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_digital_1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    dropdown_digital_2 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_digital_2, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_digital_3 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_digital_3, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    dropdown_digital_4 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_digital_4, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    dropdown_digital_5 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_digital_5, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
}

/*
 ********************************************************************************************************
 *@func       TabUSB_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabUSB_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "输出模式");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "输出时间");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_usb_2 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_usb_2, true);
    lv_textarea_set_align(text_usb_2, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_usb_2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_usb_1 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_usb_1, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
}

/*
 ********************************************************************************************************
 *@func       TabSystem_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabSystem_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "语言");

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "日期");

    lv_obj_t *label3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label3, &my_font_14, 0);
    lv_label_set_text(label3, "时间");

    lv_obj_t *label4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label4, &my_font_14, 0);
    lv_label_set_text(label4, "背光");

    lv_obj_t *label5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label5, &my_font_14, 0);
    lv_label_set_text(label5, "点亮时间");

    lv_obj_t *label6 = lv_label_create(parent);
    lv_obj_set_style_text_font(label6, &my_font_14, 0);
    lv_label_set_text(label6, "亮度");

    lv_obj_t *label7 = lv_label_create(parent);
    lv_obj_set_style_text_font(label7, &my_font_14, 0);
    lv_label_set_text(label7, "密码");

    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label5, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(label6, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(label7, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    dropdown_sys_1 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_sys_1, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_sys_2 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sys_2, true);
    lv_textarea_set_align(text_sys_2, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_sys_2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_sys_3 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sys_3, true);
    lv_textarea_set_align(text_sys_3, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_sys_3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    dropdown_sys_4 = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_sys_4, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_sys_5 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sys_5, true);
    lv_textarea_set_align(text_sys_5, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_sys_5, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    // slider_sys_6 = lv_slider_create(parent);
    // lv_obj_set_grid_cell(slider_sys_6, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    text_sys_7 = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sys_7, true);
    lv_textarea_set_align(text_sys_7, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_sys_7, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
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
    tabMeas = lv_tabview_add_tab(contBody, "测量设置");
    tabRelay = lv_tabview_add_tab(contBody, "继电器设置");
    tabAnalog = lv_tabview_add_tab(contBody, "模拟输出设置");
    tabDigital = lv_tabview_add_tab(contBody, "数字输出设置");
    tabUSB = lv_tabview_add_tab(contBody, "USB输出设置");
    tabSystem = lv_tabview_add_tab(contBody, "系统设置");
    tabPasswd = lv_tabview_add_tab(contBody, "密码管理");
    tabReset = lv_tabview_add_tab(contBody, "重置");

    TabMeas_Creat(tabMeas);
    TabRelay_Creat(tabRelay);
    TabAnalog_Creat(tabAnalog);
    TabDigital_Creat(tabDigital);
    TabUSB_Creat(tabUSB);
    // TabSystem_Creat(tabSystem);
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

    tab_index = SET_MEAS;
    tab_name_refresh();
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

    /*删除此页面上的子控件*/
    lv_obj_clean(appWindow);
}

/*
 ********************************************************************************************************
 *@brief      页面控制器
 ********************************************************************************************************
 */
Page_TypeDef page_Set = {
    .pageID = PAGE_SET,
    .exitCallback = Exit,
    .setupCallback = Setup,
    .next = NULL,
};

/*
 ********************************************************************************************************
 *@func       PageRegister_Set
 *@brief      页面注册
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
void PageRegister_Set(Page_Type pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    /*注册至页面调度器*/
    Page_Register(&page_Set);
}