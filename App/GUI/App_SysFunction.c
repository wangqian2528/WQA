/*
*********************************************************************************************************
*    @Brief   : GUI所需的系统函数
*    @Name    : App_SysFunction.h
*    @Version : V1.0
*    @Note    : GUIX的系统API
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include "gui_task.h"

/*
*********************************************************************************************************
*                                           动态内存分配                                                
*********************************************************************************************************
*/
#define       GUI_NUMBYTES   212*1024                               /* 设置动态内存大小 */
TX_BYTE_POOL  memory_pool;
uint8_t       *MemoryBlock = (uint8_t *)(0x24000000 + 1024*300);    /* 动态内存地址    */
#define       Canvas_Memory  0x24025800                             /* 设置Canvas地址   */

/*
*********************************************************************************************************
*                                           变量
*********************************************************************************************************
*/
extern GX_STUDIO_DISPLAY_INFO GUI_display_table[1];

/*
*********************************************************************************************************
*                                          动态内存函数
*********************************************************************************************************
*/
VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

/*
*********************************************************************************************************
*    函 数 名: gx_initconfig
*    功能说明: GUIX
*    形    参: 无       
*    返 回 值: 无
*********************************************************************************************************
*/
void gx_initconfig(void) 
{
     /* 初始化内存池 */
    tx_byte_pool_create(&memory_pool, "MemoryBlock", MemoryBlock, GUI_NUMBYTES);
    
    /* 初始化GUIX */
    gx_system_initialize();

    /* 注册动态内存申请和释放函数 */
    gx_system_memory_allocator_set(memory_allocate, memory_free);

    GUI_display_table[0].x_resolution = 320;
    GUI_display_table[0].y_resolution = 240;
    GUI_display_table[0].canvas_memory = (GX_COLOR *)Canvas_Memory;
}
