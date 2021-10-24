/******************************************************************************
 * @brief    显示基础模块
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

/*实例化页面调度器*/
PageManager_TypeDef page_manager = {
    .head = NULL,
    .lastPage = PAGE_NONE,
    .nextPage = PAGE_NONE,
    .nowPage = PAGE_NONE,
};

/*
 ********************************************************************************************************
 *@func       Page_Register
 *@brief      页面注册器
 *@param[in]  node - 页面子节点
 *@retval     none
 ********************************************************************************************************
 */
void Page_Register(Page_TypeDef *node)
{
    Page_TypeDef *tail = page_manager.head;
    if (page_manager.head == NULL)
    {
        page_manager.head = node;
        return;
    }
    while (tail->next != NULL)
        tail = tail->next;
    tail->next = node;
}

/*
 ********************************************************************************************************
 *@func       Page_Init
 *@brief      页面初始化
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
void Page_Init(void)
{
    PageRegister_Start(PAGE_START);
    PageRegister_Home(PAGE_HOME);
    PageRegister_Set(PAGE_SET);

    Page_ChangeTo(PAGE_SET);
}

/*
 ********************************************************************************************************
 *@func       Page_Get
 *@brief      获取页面实例
 *@param[in]  page_id - 页面编号
 *@retval     return - Page指针
 ********************************************************************************************************
 */
Page_TypeDef *Page_GetObj(Page_Type page_id)
{
    if (!IS_PAGE(page_id))
        return NULL;
    if (page_manager.head == NULL)
        return NULL;

    Page_TypeDef *tail = page_manager.head;
    while (tail->pageID != page_id)
    {
        if (tail->next == NULL)
            return NULL;
        else
            tail = tail->next;
    }
    return tail;
}

/*
 ********************************************************************************************************
 *@func       Page_ChangeTo
 *@brief      页面切换
 *@param[in]  page_id - 页面编号
              page_stay - 目标页面停留时间
 *@retval     none
 ********************************************************************************************************
 */
void Page_ChangeTo(Page_Type page_id)
{
    if (!IS_PAGE(page_id))
        return;
    if (page_id != page_manager.nowPage)
        page_manager.nextPage = page_id;
}

/*
 ********************************************************************************************************
 *@func       Page_Running
 *@brief      页面调度器
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
void Page_Running(void)
{
    Page_TypeDef *node;
    if (page_manager.nextPage != PAGE_NONE) /*页面切换事件*/
    {
        /*触发旧页面退出事件*/
        if (IS_PAGE(page_manager.nowPage))
        {
            node = Page_GetObj(page_manager.nowPage);
            node->exitCallback();
        }
        /*标记旧页面*/
        page_manager.lastPage = page_manager.nowPage;
        /*标记新页面*/
        page_manager.nowPage = page_manager.nextPage;
        page_manager.nextPage = PAGE_NONE;

        /*触发新页面初始化事件*/
        if (IS_PAGE(page_manager.nowPage))
        {
            node = Page_GetObj(page_manager.nowPage);
            node->setupCallback();
        }
    }
}