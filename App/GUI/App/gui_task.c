/*
*********************************************************************************************************
*    @Brief   : GUI界面主函数
*    @Name    : gui_task.h
*    @Version : V1.0
*    @Note    : GUI界面主函数
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include "gui_task.h"

/*
*********************************************************************************************************
*                                               变量
*********************************************************************************************************
*/
GX_WINDOW         *pScreen;
GX_WINDOW_ROOT    *root;

/*
*********************************************************************************************************
*    函 数 名: MainTask
*    功能说明: GUI主函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void GuiTask(void)
{
    /*初始化配置 */
    gx_initconfig();

    /* 配置显示屏 */
    gx_studio_display_configure(DISPLAY_1, stm32h7_graphics_driver_setup_565rgb, LANGUAGE_CHINESE, DISPLAY_1_THEME_1, &root);

    /* 创建窗口 */
    gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
    gx_studio_named_widget_create("window_2", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
    /* 显示根窗口 */
    gx_widget_show(root);

    /* 启动GUIX */
    gx_system_start();
    tx_thread_sleep(100);
    
    while(1)
    {
        tx_thread_sleep(20);
    }
}
