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
    MRO_SET = 0,
    MRO_OUTPUT,
    MRO_MAX,
} MRO_NAME_E;

const char *mro_name[] = {
    "维护设置",
    "测试输出",
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

static MRO_NAME_E tab_index;

/*子窗口*/
static lv_obj_t *tabSet;
static lv_obj_t *tabOutput;

//Set
static lv_obj_t *dropdown_set_mod;
static lv_obj_t *text_set_time;
static lv_obj_t *dropdown_set_time;
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
    if (tab_index == MRO_SET)
    {
        lv_label_set_text(labelNextTabName, mro_name[MRO_OUTPUT]);
        lv_label_set_text(labelLastTabName, mro_name[MRO_OUTPUT]);
    }
    else if (tab_index == MRO_OUTPUT)
    {
        lv_label_set_text(labelNextTabName, mro_name[MRO_SET]);
        lv_label_set_text(labelLastTabName, mro_name[MRO_SET]);
    }
    lv_label_set_text(labelTabName, mro_name[tab_index]);
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
        if (tab_index == MRO_SET)
            tab_index = MRO_OUTPUT;
        else
            tab_index = MRO_SET;
        tab_name_refresh();
    }
    else if (obj == btnRight)
    {
        if (tab_index == MRO_SET)
            tab_index = MRO_OUTPUT;
        else
            tab_index = MRO_SET;
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
 *@func       TabInstr_Creat
 *@brief      创建子窗口
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void TabSet_Creat(lv_obj_t *parent)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_style_pad_row(parent, 5, 0);

    lv_obj_t *label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, &my_font_14, 0);
    lv_label_set_text(label1, "清洗模式");
    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, &my_font_14, 0);
    lv_label_set_text(label2, "定时清洗时间");
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_set_mod = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_set_mod, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    text_set_time = lv_textarea_create(parent);
    lv_textarea_set_one_line(text_set_time, true);
    lv_textarea_set_align(text_set_time, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_grid_cell(text_set_time, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    dropdown_set_time = lv_dropdown_create(parent);
    lv_obj_set_grid_cell(dropdown_set_time, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
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
    tabSet = lv_tabview_add_tab(contBody, "维护设置");
    tabOutput = lv_tabview_add_tab(contBody, "测试输出");

    TabSet_Creat(tabSet);
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

    tab_index = MRO_SET;
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
Page_TypeDef page_Mro = {
    .pageID = PAGE_MRO,
    .exitCallback = Exit,
    .setupCallback = Setup,
    .next = NULL,
};

/*
 ********************************************************************************************************
 *@func       PageRegister_Mro
 *@brief      页面注册
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
void PageRegister_Mro(Page_Type pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    /*注册至页面调度器*/
    Page_Register(&page_Mro);
}