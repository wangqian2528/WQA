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
    TAB_SENSOR = 0,
    TAB_RELAY,
    TAB_ANALOG,
    TAB_CAL,
    TAB_VER,
    TAB_ERROR,
    TAB_MAX,
} TAB_NAME_E;

const char *tab_name[] = {
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

static TAB_NAME_E tab_index;

/*子窗口*/
static lv_obj_t *tabSensor;
static lv_obj_t *tabRelay;
static lv_obj_t *tabAnalog;
static lv_obj_t *tabCal;
static lv_obj_t *tabVer;
static lv_obj_t *tabError;

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
    if (tab_index == TAB_SENSOR)
    {
        lv_label_set_text(labelNextTabName, tab_name[TAB_RELAY]);
        lv_label_set_text(labelLastTabName, tab_name[TAB_ERROR]);
    }
    else if (tab_index == TAB_ERROR)
    {
        lv_label_set_text(labelNextTabName, tab_name[TAB_SENSOR]);
        lv_label_set_text(labelLastTabName, tab_name[TAB_VER]);
    }
    else
    {
        lv_label_set_text(labelNextTabName, tab_name[tab_index + 1]);
        lv_label_set_text(labelLastTabName, tab_name[tab_index - 1]);
    }
    lv_label_set_text(labelTabName, tab_name[tab_index]);
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
        if (tab_index > TAB_SENSOR)
            tab_index--;
        else
            tab_index = TAB_ERROR;
        tab_name_refresh();
    }
    else if (obj == btnRight)
    {
        if (tab_index < TAB_ERROR)
            tab_index++;
        else
            tab_index = TAB_SENSOR;
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

    tab_index = TAB_SENSOR;
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