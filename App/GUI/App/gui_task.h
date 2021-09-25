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

#ifndef __GUI_TASK_H
#define __GUI_TASK_H

#include "bsp.h"
#include "gx_user.h"

#include "GUI_resources.h"
#include "GUI_specifications.h"

/*
************************************************************************
*                               宏定义
************************************************************************
*/



/*
************************************************************************
*                           供外部文件调用
************************************************************************
*/
extern void GuiTask(void);
extern void gx_initconfig(void);
extern UINT stm32h7_graphics_driver_setup_565rgb(GX_DISPLAY *display);

#endif
