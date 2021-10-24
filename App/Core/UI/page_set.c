/******************************************************************************
 * @brief    设置页面
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

/*返回按键*/
static lv_obj_t *imgbtnBack;

/*路径显示*/
static lv_obj_t *labelTitle;

/*设置菜单*/
static lv_obj_t *tabviewSet;

static lv_obj_t *tabMeasure;
static lv_obj_t *tabRelay;
static lv_obj_t *tabAanlog;
static lv_obj_t *tabDigital;
static lv_obj_t *tabUSB;
static lv_obj_t *tabSystem;
static lv_obj_t *tabPassWD;
static lv_obj_t *tabReset;

/*测量设置页面*/
static lv_obj_t *labelPara;       //测量参数
static lv_obj_t *labelSensor;     //传感器
static lv_obj_t *labelTempS;      //温度源
static lv_obj_t *labelMode;       //测量工作模式
static lv_obj_t *labelTime;       //定时测量时间
static lv_obj_t *labelFilter;     //滤波时间
static lv_obj_t *labelTemp;       //MTC温度值
static lv_obj_t *labelTempCom;    //温度补偿值
static lv_obj_t *labelGear;       //档位选择
static lv_obj_t *labelTempComMod; //温度补偿模式
static lv_obj_t *labelLinerCom;   //线性补偿值
static lv_obj_t *labelPwrFreq;    //市电频率

static lv_obj_t *btnReadPara;
static lv_obj_t *btnWritPara;
static lv_obj_t *labelBtnRead;
static lv_obj_t *labelBtnWrit;

static lv_obj_t *ddlistPara;       //测量参数
static lv_obj_t *ddlistSensor;     //传感器
static lv_obj_t *ddlistTempS;      //温度源
static lv_obj_t *ddlistMode;       //测量工作模式
static lv_obj_t *taTimeVal;        //定时测量时间
static lv_obj_t *ddlistTimeUnit;   //定时测量时间
static lv_obj_t *taFilter;         //滤波时间
static lv_obj_t *taTemp;           //MTC温度值
static lv_obj_t *ddlistTempUnit;   //MTC温度值
static lv_obj_t *taTempCom;        //温度补偿值
static lv_obj_t *ddlistGear;       //档位选择
static lv_obj_t *ddlistTempComMod; //温度补偿模式
static lv_obj_t *taLinerCom;       //线性补偿值
static lv_obj_t *ddlistPwrFreq;    //市电频率

/*选项字符串*/
const char *title_string[] = {
    "测量",
    "继电器",
    "模拟输出",
    "数字输出",
    "USB",
    "系统",
    "密码管理",
    "重置",
};

/*
 ********************************************************************************************************
 *@func       tabview_event_cb
 *@brief      菜单选项卡事件回调
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
void tabview_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (obj == tabviewSet && event == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t index = lv_tabview_get_tab_act(tabviewSet);
        lv_label_set_text_fmt(labelTitle, "%s-%s", "设置", title_string[index]);
    }
}

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
        if (obj == imgbtnBack)
        {
            Page_ChangeTo(PAGE_HOME);
        }
    }
}

/*
 ********************************************************************************************************
 *@func       SetMeasure_Creat
 *@brief      创建测量设置页面
 *@param[in]  node
 *@retval     none
 ********************************************************************************************************
 */
static void SetMeasure_Creat(void)
{
    //label创建
    LV_FONT_DECLARE(SYHT_MED_16);
    labelPara = lv_label_create(tabMeasure, NULL);
    static lv_style_t label_measure_style;
    label_measure_style = *lv_label_get_style(labelPara, LV_LABEL_STYLE_MAIN);
    label_measure_style.text.font = &SYHT_MED_16;
    lv_label_set_style(labelPara, LV_LABEL_STYLE_MAIN, &label_measure_style);
    lv_obj_align(labelPara, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 15);
    lv_obj_set_auto_realign(labelPara, true);
    lv_label_set_text(labelPara, "测量参数");

    labelSensor = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelSensor, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 45);
    lv_obj_set_auto_realign(labelSensor, true);
    lv_label_set_text(labelSensor, "传感器");

    labelTempS = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelTempS, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 75);
    lv_obj_set_auto_realign(labelTempS, true);
    lv_label_set_text(labelTempS, "温度源");

    labelMode = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelMode, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 105);
    lv_obj_set_auto_realign(labelMode, true);
    lv_label_set_text(labelMode, "测量工作模式");

    labelTime = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelTime, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 135);
    lv_obj_set_auto_realign(labelTime, true);
    lv_label_set_text(labelTime, "定时测量时间");

    labelFilter = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelFilter, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 165);
    lv_obj_set_auto_realign(labelFilter, true);
    lv_label_set_text(labelFilter, "滤波时间");

    labelTemp = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelTemp, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 195);
    lv_obj_set_auto_realign(labelTemp, true);
    lv_label_set_text(labelTemp, "MTC温度值");

    labelTempCom = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelTempCom, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 225);
    lv_obj_set_auto_realign(labelTempCom, true);
    lv_label_set_text(labelTempCom, "温度补偿值");

    labelGear = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelGear, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 255);
    lv_obj_set_auto_realign(labelGear, true);
    lv_label_set_text(labelGear, "档位选择");

    labelTempComMod = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelTempComMod, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 285);
    lv_obj_set_auto_realign(labelTempComMod, true);
    lv_label_set_text(labelTempComMod, "温度补偿模式");

    labelLinerCom = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelLinerCom, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 315);
    lv_obj_set_auto_realign(labelLinerCom, true);
    lv_label_set_text(labelLinerCom, "线性补偿值");

    labelPwrFreq = lv_label_create(tabMeasure, labelPara);
    lv_obj_align(labelPwrFreq, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 345);
    lv_obj_set_auto_realign(labelPwrFreq, true);
    lv_label_set_text(labelPwrFreq, "市电频率");

    btnReadPara = lv_btn_create(tabMeasure, NULL);
    lv_obj_set_size(btnReadPara, 75, 40);
    labelBtnRead = lv_label_create(btnReadPara, NULL);
    static lv_style_t btn_read_style_pr, btn_read_style_rel;
    btn_read_style_pr = *lv_btn_get_style(btnReadPara, LV_BTN_STYLE_PR);
    btn_read_style_rel = *lv_btn_get_style(btnReadPara, LV_BTN_STYLE_REL);
    btn_read_style_pr.text.font = &SYHT_MED_16;
    btn_read_style_rel.text.font = &SYHT_MED_16;
    lv_btn_set_style(btnReadPara, LV_BTN_STYLE_PR, &btn_read_style_pr);
    lv_btn_set_style(btnReadPara, LV_BTN_STYLE_REL, &btn_read_style_rel);
    lv_btn_set_layout(btnReadPara, LV_LAYOUT_OFF);
    lv_label_set_text(labelBtnRead, "读取参数");
    lv_obj_align(btnReadPara, NULL, LV_ALIGN_IN_TOP_LEFT, 50, 375);
    lv_obj_align(labelBtnRead, NULL, LV_ALIGN_CENTER, 0, 5);
    lv_obj_set_auto_realign(btnReadPara, true);

    btnWritPara = lv_btn_create(tabMeasure, btnReadPara);
    labelBtnWrit = lv_label_create(btnWritPara, NULL);
    lv_btn_set_layout(btnWritPara, LV_LAYOUT_OFF);
    lv_label_set_text(labelBtnWrit, "写入参数");
    lv_obj_align(btnWritPara, NULL, LV_ALIGN_IN_TOP_LEFT, 190, 375);
    lv_obj_align(labelBtnWrit, NULL, LV_ALIGN_CENTER, 0, 5);
    lv_obj_set_auto_realign(btnWritPara, true);

    //控件创建
    ddlistPara = lv_ddlist_create(tabMeasure, NULL);
    static lv_style_t ddlist_para_style_bg, ddlist_para_style_sl;
    ddlist_para_style_bg = *lv_ddlist_get_style(ddlistPara, LV_DDLIST_STYLE_BG);
    ddlist_para_style_sl = *lv_ddlist_get_style(ddlistPara, LV_DDLIST_STYLE_SEL);
    ddlist_para_style_bg.text.font = &SYHT_MED_16;
    ddlist_para_style_sl.text.font = &SYHT_MED_16;
    lv_ddlist_set_style(ddlistPara, LV_DDLIST_STYLE_BG, &ddlist_para_style_bg);
    lv_ddlist_set_style(ddlistPara, LV_DDLIST_STYLE_SEL, &ddlist_para_style_sl);
    lv_ddlist_set_align(ddlistPara, LV_LABEL_ALIGN_CENTER);
    lv_ddlist_set_fix_width(ddlistPara, 120);
    lv_ddlist_set_options(ddlistPara, "电导度\n电导度\n电导度");
    lv_obj_align(ddlistPara, NULL, LV_ALIGN_IN_TOP_RIGHT, -15, 10);
}

// static lv_obj_t *ddlistPara;       //测量参数
// static lv_obj_t *ddlistSensor;     //传感器
// static lv_obj_t *ddlistTempS;      //温度源
// static lv_obj_t *ddlistMode;       //测量工作模式
// static lv_obj_t *taTimeVal;        //定时测量时间
// static lv_obj_t *ddlistTimeUnit;   //定时测量时间
// static lv_obj_t *taFilter;         //滤波时间
// static lv_obj_t *taTemp;           //MTC温度值
// static lv_obj_t *ddlistTempUnit;   //MTC温度值
// static lv_obj_t *taTempCom;        //温度补偿值
// static lv_obj_t *ddlistGear;       //档位选择
// static lv_obj_t *ddlistTempComMod; //温度补偿模式
// static lv_obj_t *taLinerCom;       //线性补偿值
// static lv_obj_t *ddlistPwrFreq;    //市电频率

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

    /*返回按键*/
    LV_IMG_DECLARE(IconBackW);
    LV_IMG_DECLARE(IconBackB);
    imgbtnBack = lv_imgbtn_create(contTitle, NULL);
    lv_imgbtn_set_src(imgbtnBack, LV_BTN_STATE_REL, &IconBackW);
    lv_imgbtn_set_src(imgbtnBack, LV_BTN_STATE_PR, &IconBackB);
    lv_obj_align(imgbtnBack, contTitle, LV_ALIGN_IN_LEFT_MID, 3, 3);
    lv_obj_set_event_cb(imgbtnBack, btn_event_cb);

    /*路径显示*/
    LV_FONT_DECLARE(SYHT_BOLD_20);
    labelTitle = lv_label_create(contTitle, NULL);
    static lv_style_t label_title_style;
    label_title_style = *lv_label_get_style(labelTitle, LV_LABEL_STYLE_MAIN);
    label_title_style.text.font = &SYHT_BOLD_20;
    label_title_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(labelTitle, LV_LABEL_STYLE_MAIN, &label_title_style);
    lv_obj_align(labelTitle, imgbtnBack, LV_ALIGN_OUT_RIGHT_MID, 3, 5);
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
    //菜单选项卡
    tabviewSet = lv_tabview_create(appWindow, NULL);
    lv_obj_set_size(tabviewSet, 320, 200);
    lv_obj_align(tabviewSet, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_tabview_set_btns_pos(tabviewSet, LV_TABVIEW_BTNS_POS_BOTTOM);
    static lv_style_t tabview_set_style;
    tabview_set_style = *lv_tabview_get_style(tabviewSet, LV_TABVIEW_STYLE_BG);
    tabview_set_style.body.radius = 0;
    lv_tabview_set_style(tabviewSet, LV_TABVIEW_STYLE_BG, &tabview_set_style);

    tabMeasure = lv_tabview_add_tab(tabviewSet, " ");
    tabRelay = lv_tabview_add_tab(tabviewSet, " ");
    tabAanlog = lv_tabview_add_tab(tabviewSet, " ");
    tabDigital = lv_tabview_add_tab(tabviewSet, " ");
    tabUSB = lv_tabview_add_tab(tabviewSet, " ");
    tabSystem = lv_tabview_add_tab(tabviewSet, " ");
    tabPassWD = lv_tabview_add_tab(tabviewSet, " ");
    tabReset = lv_tabview_add_tab(tabviewSet, " ");

    lv_obj_set_event_cb(tabviewSet, tabview_event_cb);
    tabview_event_cb(tabviewSet, LV_EVENT_VALUE_CHANGED);
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
    ContTitle_Creat();
    Body_Creat();
    SetMeasure_Creat();

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