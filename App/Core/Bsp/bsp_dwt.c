/******************************************************************************
 * @brief    数据观察点与跟踪(DWT)模块
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
*                                             寄存器
*********************************************************************************************************
*/
#define DEM_CR_TRCENA (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)

/*
 ********************************************************************************************************
 * @func        bsp_InitDWT
 * @brief       初始化DWT
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_InitDWT(void)
{
    DEM_CR |= (unsigned int)DEM_CR_TRCENA;
    DWT_CYCCNT = (unsigned int)0u;
    DWT_CR |= (unsigned int)DWT_CR_CYCCNTENA;
}

/*
 ********************************************************************************************************
 * @func        bsp_DelayUS
 * @brief       延时采用CPU的内部计数实现，32位计数器
 *              OSSchedLock(&err);
 *              bsp_DelayUS(5);
 *              OSSchedUnlock(&err); 根据实际情况看看是否需要加调度锁或选择关中断
 * @param[in]   _ulDelayTime - 延迟us
 * @return      none
 ********************************************************************************************************
 */
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
    uint32_t tStart;

    tStart = DWT_CYCCNT; /* 刚进入时的计数器值 */
    tCnt = 0;
    tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000); /* 需要的节拍数 */

    while (tCnt < tDelayCnt)
    {
        tCnt = DWT_CYCCNT - tStart; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_DelayMS
 * @brief       阻塞式的延迟函数
 * @param[in]   _ulDelayTime - 延迟ms
 * @return      none
 ********************************************************************************************************
 */
void bsp_DelayMS(uint32_t _ulDelayTime)
{
    bsp_DelayUS(1000 * _ulDelayTime);
}