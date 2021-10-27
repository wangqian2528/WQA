/******************************************************************************
 * @brief    信息页面
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
    INFO_SENSOR = 0,
    INFO_RELAY,
    INFO_ANALOG,
    INFO_CAL,
    INFO_VER,
    INFO_ERROR,
    INFO_MAX,
} INFO_NAME_E;

const char *info_name[] = {
    "传感器信息",
    "继电器信息",
    "模拟输出信息",
    "校准数据信息",
    "软硬件版本",
    "错误信息",
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

static INFO_NAME_E tab_index;

/*子窗口*/
static lv_obj_t *tabSensor;
static lv_obj_t *tabRelay;
static lv_obj_t *tabAnalog;
static lv_obj_t *tabCal;
static lv_obj_t *tabVer;
static lv_obj_t *tabError;

//sensor
static lv_obj_t *text_sensor_type;
static lv_obj_t *text_sensor_soft;
static lv_obj_t *text_sensor_hard;
static lv_obj_t *text_sensor_code;
//relay
static lv_obj_t *dropdown_relay_num;
static lv_obj_t *text_relay_para;
static lv_obj_t *text_relay_act;
static lv_obj_t *text_relay_threshold;
//analog
static lv_obj_t *dropdown_analog_num;
static lv_obj_t *text_analog_para;
static lv_obj_t *text_analog_range;
static lv_obj_t *text_analog_max;
static lv_obj_t *text_analog_min;
//cal
static lv_obj_t *text_cal_ceff;
//ver
static lv_obj_t *text_ver_dev;
static lv_obj_t *text_ver_serial;
static lv_obj_t *text_ver_soft;
static lv_obj_t *text_ver_hard;

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
    if (tab_index == INFO_SENSOR)
    {
        lv_label_set_text(labelNextTabName, info_name[INFO_RELAY]);
        lv_label_set_text(labelLastTabName, info_name[INFO_ERROR]);
    }
    else if (tab_index == INFO_ERROR)
    {
        lv_label_set_text(labelNextTabName, info_name[INFO_SENSOR]);
        lv_label_set_text(labelLastTabName, info_name[INFO_VER]);
    }
    else
    {
        lv_label_set_text(labelNextTabName, info_name[tab_index + 1]);
        lv_label_set_text(labelLastTabName, info_name[tab_index - 1]);
    }
    lv_label_set_text(labelTabName, info_name[tab_index]);
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
        if (tab_index > INFO_SENSOR)
            tab_index--;
        else
            tab_index = INFO_ERROR;
        tab_name_refresh();
    }
    else if (obj == btnRight)
    {
        if (tab_index < INFO_ERROR)
            tab_index++;
        else
            tab_index = INFO_SENSOR;
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
 *@func       TabSensor_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabSensor_Creat(lv_obj_t *parent)
{
    //tabSensor
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    //OBJ
    lv_obj_t *label_type = lv_label_create(parent);
    lv_obj_set_style_text_font(label_type, &my_font_14, 0);
    lv_label_set_text(label_type, "类型");

    lv_obj_t *label_soft = lv_label_create(parent);
    lv_obj_set_style_text_font(label_soft, &my_font_14, 0);
    lv_label_set_text(label_soft, "软件版本");

    lv_obj_t *label_hard = lv_label_create(parent);
    lv_obj_set_style_text_font(label_hard, &my_font_14, 0);
    lv_label_set_text(label_hard, "硬件版本");

    lv_obj_t *label_code = lv_label_create(parent);
    lv_obj_set_style_text_font(label_code, &my_font_14, 0);
    lv_label_set_text(label_code, "序列号");

    lv_obj_set_grid_cell(label_type, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label_soft, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label_hard, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label_code, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_sensor_type = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sensor_type, true);
    lv_obj_clear_flag(text_sensor_type, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_font(text_sensor_type, &my_font_14, 0);
    lv_obj_set_style_text_opa(text_sensor_type, LV_OPA_60, 0);
    lv_textarea_set_text(text_sensor_type, "数字");
    lv_textarea_set_align(text_sensor_type, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_sensor_type, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_sensor_soft = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sensor_soft, true);
    lv_obj_clear_flag(text_sensor_soft, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_sensor_soft, LV_OPA_60, 0);
    lv_textarea_set_text(text_sensor_soft, "V1.0.0");
    lv_textarea_set_align(text_sensor_soft, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_sensor_soft, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_sensor_hard = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sensor_hard, true);
    lv_obj_clear_flag(text_sensor_hard, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_sensor_hard, LV_OPA_60, 0);
    lv_textarea_set_text(text_sensor_hard, "V1.0.0");
    lv_textarea_set_align(text_sensor_hard, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_sensor_hard, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_sensor_code = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_sensor_code, true);
    lv_obj_clear_flag(text_sensor_code, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_sensor_code, LV_OPA_60, 0);
    lv_textarea_set_text(text_sensor_code, "1234567890");
    lv_textarea_set_align(text_sensor_code, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_sensor_code, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
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
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    //OBJ
    lv_obj_t *label_num = lv_label_create(parent);
    lv_obj_set_style_text_font(label_num, &my_font_14, 0);
    lv_label_set_text(label_num, "继电器组");

    lv_obj_t *label_para = lv_label_create(parent);
    lv_obj_set_style_text_font(label_para, &my_font_14, 0);
    lv_label_set_text(label_para, "测量参数");

    lv_obj_t *label_act = lv_label_create(parent);
    lv_obj_set_style_text_font(label_act, &my_font_14, 0);
    lv_label_set_text(label_act, "动作类型");

    lv_obj_t *label_threshold = lv_label_create(parent);
    lv_obj_set_style_text_font(label_threshold, &my_font_14, 0);
    lv_label_set_text(label_threshold, "动作阈值");

    lv_obj_set_grid_cell(label_num, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label_para, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label_act, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label_threshold, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    dropdown_relay_num = lv_dropdown_create(parent);
    lv_dropdown_set_options_static(dropdown_relay_num, "#1\n#2\n#3\n#4");
    lv_obj_set_grid_cell(dropdown_relay_num, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_relay_para = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_para, true);
    lv_obj_clear_flag(text_relay_para, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_relay_para, LV_OPA_60, 0);
    lv_textarea_set_align(text_relay_para, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_relay_para, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_relay_act = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_act, true);
    lv_obj_clear_flag(text_relay_act, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_relay_act, LV_OPA_60, 0);
    lv_textarea_set_align(text_relay_act, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_relay_act, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_relay_threshold = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_relay_threshold, true);
    lv_obj_clear_flag(text_relay_threshold, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_relay_threshold, LV_OPA_60, 0);
    lv_textarea_set_align(text_relay_threshold, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_relay_threshold, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
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
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    //OBJ
    lv_obj_t *label_num = lv_label_create(parent);
    lv_obj_set_style_text_font(label_num, &my_font_14, 0);
    lv_label_set_text(label_num, "模拟输出序号");

    lv_obj_t *label_para = lv_label_create(parent);
    lv_obj_set_style_text_font(label_para, &my_font_14, 0);
    lv_label_set_text(label_para, "测量参数");

    lv_obj_t *label_range = lv_label_create(parent);
    lv_obj_set_style_text_font(label_range, &my_font_14, 0);
    lv_label_set_text(label_range, "输出范围");

    lv_obj_t *label_max = lv_label_create(parent);
    lv_obj_set_style_text_font(label_max, &my_font_14, 0);
    lv_label_set_text(label_max, "最大值");

    lv_obj_t *label_min = lv_label_create(parent);
    lv_obj_set_style_text_font(label_min, &my_font_14, 0);
    lv_label_set_text(label_min, "最小值");

    lv_obj_set_grid_cell(label_num, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label_para, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label_range, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label_max, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(label_min, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    dropdown_analog_num = lv_dropdown_create(parent);
    lv_dropdown_set_options_static(dropdown_analog_num, "#1\n#2\n#3\n#4");
    lv_obj_set_grid_cell(dropdown_analog_num, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_analog_para = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_para, true);
    lv_obj_clear_flag(text_analog_para, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_analog_para, LV_OPA_60, 0);
    lv_textarea_set_align(text_analog_para, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_analog_para, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_analog_range = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_range, true);
    lv_obj_clear_flag(text_analog_range, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_analog_range, LV_OPA_60, 0);
    lv_textarea_set_align(text_analog_range, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_analog_range, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_analog_max = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_max, true);
    lv_obj_clear_flag(text_analog_max, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_analog_max, LV_OPA_60, 0);
    lv_textarea_set_align(text_analog_max, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_analog_max, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_analog_min = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_analog_min, true);
    lv_obj_clear_flag(text_analog_min, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_analog_min, LV_OPA_60, 0);
    lv_textarea_set_align(text_analog_min, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_analog_min, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
}

/*
 ********************************************************************************************************
 *@func       TabCal_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabCal_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label_coeff = lv_label_create(parent);
    lv_obj_set_style_text_font(label_coeff, &my_font_14, 0);
    lv_label_set_text(label_coeff, "电极系数");
    lv_obj_set_grid_cell(label_coeff, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_cal_ceff = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_cal_ceff, true);
    lv_obj_clear_flag(text_cal_ceff, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_cal_ceff, LV_OPA_60, 0);
    lv_textarea_set_align(text_cal_ceff, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_cal_ceff, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
}

/*
 ********************************************************************************************************
 *@func       TabVer_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabVer_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    //OBJ
    lv_obj_t *label_dev_model = lv_label_create(parent);
    lv_obj_set_style_text_font(label_dev_model, &my_font_14, 0);
    lv_label_set_text(label_dev_model, "仪器型号");

    lv_obj_t *label_serial_num = lv_label_create(parent);
    lv_obj_set_style_text_font(label_serial_num, &my_font_14, 0);
    lv_label_set_text(label_serial_num, "序列号");

    lv_obj_t *label_soft_ver = lv_label_create(parent);
    lv_obj_set_style_text_font(label_soft_ver, &my_font_14, 0);
    lv_label_set_text(label_soft_ver, "软件版本");

    lv_obj_t *label_hard_ver = lv_label_create(parent);
    lv_obj_set_style_text_font(label_hard_ver, &my_font_14, 0);
    lv_label_set_text(label_hard_ver, "硬件版本");

    lv_obj_set_grid_cell(label_dev_model, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(label_serial_num, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(label_soft_ver, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(label_hard_ver, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    text_ver_dev = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_ver_dev, true);
    lv_obj_clear_flag(text_ver_dev, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_ver_dev, LV_OPA_60, 0);
    lv_textarea_set_align(text_ver_dev, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_ver_dev, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_ver_serial = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_ver_serial, true);
    lv_obj_clear_flag(text_ver_serial, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_ver_serial, LV_OPA_60, 0);
    lv_textarea_set_align(text_ver_serial, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_ver_serial, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    text_ver_soft = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_ver_soft, true);
    lv_obj_clear_flag(text_ver_soft, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_ver_soft, LV_OPA_60, 0);
    lv_textarea_set_align(text_ver_soft, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_ver_soft, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    text_ver_hard = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_ver_hard, true);
    lv_obj_clear_flag(text_ver_hard, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_opa(text_ver_hard, LV_OPA_60, 0);
    lv_textarea_set_align(text_ver_hard, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_grid_cell(text_ver_hard, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
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
    tabSensor = lv_tabview_add_tab(contBody, "传感器信息");
    tabRelay = lv_tabview_add_tab(contBody, "继电器信息");
    tabAnalog = lv_tabview_add_tab(contBody, "模拟输出信息");
    tabCal = lv_tabview_add_tab(contBody, "校准数据信息");
    tabVer = lv_tabview_add_tab(contBody, "软硬件版本");
    tabError = lv_tabview_add_tab(contBody, "错误信息");

    TabSensor_Creat(tabSensor);
    TabRelay_Creat(tabRelay);
    TabAnalog_Creat(tabAnalog);
    TabCal_Creat(tabCal);
    TabVer_Creat(tabVer);
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

    tab_index = INFO_SENSOR;
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
Page_TypeDef page_Info = {
    .pageID = PAGE_INFO,
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
void PageRegister_Info(Page_Type pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    /*注册至页面调度器*/
    Page_Register(&page_Info);
}