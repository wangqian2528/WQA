/******************************************************************************
 * @brief    threadx任务
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

/*
*********************************************************************************************************
*                                 任务优先级，数值越小优先级越高
*********************************************************************************************************
*/
#define APP_CFG_TASK_START_PRIO 2u
#define APP_CFG_TASK_STAT_PRIO 30u
#define APP_CFG_TASK_IDLE_PRIO 31u

/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define APP_CFG_TASK_START_STK_SIZE 2048
#define APP_CFG_TASK_IDLE_STK_SIZE 1024u
#define APP_CFG_TASK_STAT_STK_SIZE 1024u

/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/
static TX_THREAD AppTaskStartTCB;
static uint64_t AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 8];

static TX_THREAD AppTaskIdleTCB;
static uint64_t AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE / 8];

static TX_THREAD AppTaskStatTCB;
static uint64_t AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE / 8];

/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static void AppTaskStart(ULONG thread_input);
static void AppTaskIdle(ULONG thread_input);
static void AppTaskStat(ULONG thread_input);

static void AppTaskCreate(void);
static void AppObjCreate(void);
static void OSStatInit(void);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
static TX_MUTEX AppPrintfSemp; /* 用于printf互斥 */

/* 统计任务使用 */
__IO uint8_t OSStatRdy;  /* 统计任务就绪标志 */
__IO uint32_t OSIdleCtr; /* 空闲任务计数 */
__IO float OSCPUUsage;   /* CPU百分比 */
uint32_t OSIdleCtrMax;   /* 1秒内最大的空闲计数 */
uint32_t OSIdleCtrRun;   /* 1秒内空闲任务当前计数 */

/*
 ********************************************************************************************************
 * @func        tx_application_define
 * @brief       ThreadX专用的任务创建，通信组件创建函数
 * @param[in]   first_unused_memory - 未使用的地址空间
 * @return      none
 ********************************************************************************************************
 */
void tx_application_define(void *first_unused_memory)
{
    /**************创建启动任务*********************/
    tx_thread_create(&AppTaskStartTCB,            /* 任务控制块地址 */
                     "App Task Start",            /* 任务名 */
                     AppTaskStart,                /* 启动任务函数地址 */
                     0,                           /* 传递给任务的参数 */
                     &AppTaskStartStk[0],         /* 堆栈基地址 */
                     APP_CFG_TASK_START_STK_SIZE, /* 堆栈空间大小 */
                     APP_CFG_TASK_START_PRIO,     /* 任务优先级*/
                     APP_CFG_TASK_START_PRIO,     /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,            /* 不开启时间片 */
                     TX_AUTO_START);              /* 创建后立即启动 */

    /**************创建统计任务*********************/
    tx_thread_create(&AppTaskStatTCB,            /* 任务控制块地址 */
                     "App Task State",           /* 任务名 */
                     AppTaskStat,                /* 启动任务函数地址 */
                     0,                          /* 传递给任务的参数 */
                     &AppTaskStatStk[0],         /* 堆栈基地址 */
                     APP_CFG_TASK_STAT_STK_SIZE, /* 堆栈空间大小 */
                     APP_CFG_TASK_STAT_PRIO,     /* 任务优先级 */
                     APP_CFG_TASK_STAT_PRIO,     /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,           /* 不开启时间片 */
                     TX_AUTO_START);             /* 创建后立即启动 */

    /**************创建空闲任务*********************/
    tx_thread_create(&AppTaskIdleTCB,            /* 任务控制块地址 */
                     "App Task Idle",            /* 任务名 */
                     AppTaskIdle,                /* 启动任务函数地址 */
                     0,                          /* 传递给任务的参数 */
                     &AppTaskIdleStk[0],         /* 堆栈基地址 */
                     APP_CFG_TASK_IDLE_STK_SIZE, /* 堆栈空间大小 */
                     APP_CFG_TASK_IDLE_PRIO,     /* 任务优先级*/
                     APP_CFG_TASK_IDLE_PRIO,     /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,           /* 不开启时间片 */
                     TX_AUTO_START);             /* 创建后立即启动 */
}

/*
 ********************************************************************************************************
 * @func        AppTaskStart
 * @brief       启动任务。
 * @param[in]   thread_input - 创建该任务时传递的形参
 * @return      none
 * @priority    2
 ********************************************************************************************************
 */
static void AppTaskStart(ULONG thread_input)
{
    (void)thread_input;
    /* 优先执行任务统计 */
    OSStatInit();
    /* 内核开启后，恢复HAL里的时间基准 */
    HAL_ResumeTick();
    /* 外设初始化 */
    bsp_Init();
    /* 创建任务 */
    AppTaskCreate();
    /* 创建任务间通信机制 */
    AppObjCreate();
    while (1)
    {
        /* 需要周期性处理的程序 1ms*/
        bsp_RunPer1ms();
        tx_thread_sleep(1);
    }
}

/*
 ********************************************************************************************************
 * @func        AppTaskStat
 * @brief       统计任务，用于实现CPU利用率的统计
 * @param[in]   thread_input - 创建该任务时传递的形参
 * @return      none
 * @priority    30
 ********************************************************************************************************
 */
void OSStatInit(void)
{
    OSStatRdy = FALSE;
    tx_thread_sleep(2u);      /* 时钟同步 */
    OSIdleCtr = 0uL;          /* 清空闲计数 */
    tx_thread_sleep(100);     /* 统计100ms内，最大空闲计数 */
    OSIdleCtrMax = OSIdleCtr; /* 保存最大空闲计数 */
    OSStatRdy = TRUE;
}

static void AppTaskStat(ULONG thread_input)
{
    (void)thread_input;

    while (OSStatRdy == FALSE)
    {
        tx_thread_sleep(200); /* 等待统计任务就绪 */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL)
    {
        OSCPUUsage = 0u;
    }

    OSIdleCtr = OSIdleCtrMax * 100uL; /* 设置初始CPU利用率 0% */

    while (1)
    {
        OSIdleCtrRun = OSIdleCtr; /* 获得100ms内空闲计数 */
        OSIdleCtr = 0uL;          /* 复位空闲计数 */
        OSCPUUsage = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        tx_thread_sleep(100); /* 每100ms统计一次 */
    }
}

/*
 ********************************************************************************************************
 * @func        AppTaskIDLE
 * @brief       空闲任务
 * @param[in]   thread_input - 创建该任务时传递的形参
 * @return      none
 * @priority    31
 ********************************************************************************************************
 */
static void AppTaskIdle(ULONG thread_input)
{
    TX_INTERRUPT_SAVE_AREA;
    (void)thread_input;

    while (1)
    {
        TX_DISABLE
        OSIdleCtr++;
        TX_RESTORE
    }
}

/*
 ********************************************************************************************************
 * @func        AppTaskCreate
 * @brief       创建应用任务
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
static void AppTaskCreate(void)
{
}

/*
 ********************************************************************************************************
 * @func        AppObjCreate
 * @brief       创建任务通讯
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
static void AppObjCreate(void)
{
    /* 创建互斥信号量 */
    tx_mutex_create(&AppPrintfSemp, "AppPrintfSemp", TX_NO_INHERIT);
}

/*
 ********************************************************************************************************
 * @func        App_Printf
 * @brief       线程安全的printf方式
 * @param[in]   同printf的参数
 * @return      none
 ********************************************************************************************************
 */
void App_Printf(const char *fmt, ...)
{
    char buf_str[200 + 1]; /* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list v_args;

    va_start(v_args, fmt);
    (void)vsnprintf((char *)&buf_str[0],
                    (size_t)sizeof(buf_str),
                    (char const *)fmt,
                    v_args);
    va_end(v_args);

    /* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);
    printf("%s", buf_str);
    tx_mutex_put(&AppPrintfSemp);
}

/*
 ********************************************************************************************************
 * @func        App_DispTaskInfoPrintf
 * @brief       将任务信息通过串口打印出来
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void DispTaskInfo(void)
{
    /* 定义一个任务控制块指针 */
    TX_THREAD *p_tcb;
    p_tcb = &AppTaskStartTCB;
    /* 打印标题 */
    App_Printf("===============================================================\r\n");
    App_Printf("OS CPU Usage = %5.2f%%\r\n", OSCPUUsage);
    App_Printf("===============================================================\r\n");
    App_Printf(" 任务优先级  任务栈大小  当前使用栈   最大栈使用       任务名\r\n");
    App_Printf("   Prio     StackSize   CurStack    MaxStack      Taskname\r\n");
    /* 遍历任务控制块列 */
    while (p_tcb != (TX_THREAD *)0)
    {
        App_Printf("   %2d        %5d      %5d       %5d      %s\r\n",
                   p_tcb->tx_thread_priority,
                   p_tcb->tx_thread_stack_size,
                   (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                   (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                   p_tcb->tx_thread_name);
        p_tcb = p_tcb->tx_thread_created_next;
        if (p_tcb == &AppTaskStartTCB)
            break;
    }
}