/******************************************************************************
 * @brief    环形缓存模块
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
 ********************************************************************************************************
 *@func       ring_buf_init
 *@brief      构造一个空环形缓冲区
 *@param[in]  r    - 环形缓冲区管理器
 *@param[in]  buf  - 数据缓冲区
 *@param[in]  len  - buf长度(必须是2的N次幂)
 *@retval     bool
 ********************************************************************************************************
 */
bool ring_buf_init(ring_buffer_t *r, uint8_t *buf, uint32_t len)
{
    r->buf = buf;
    r->size = len;
    r->front = r->rear = 0;
    return buf != NULL && (len & len - 1) == 0;
}

/*
 ********************************************************************************************************
 *@func       ring_buf_clr
 *@brief      清空环形缓冲区
 *@param[in]  r    - 环形缓冲区管理器
 *@retval     none
 ********************************************************************************************************
 */
void ring_buf_clr(ring_buffer_t *r)
{
    r->front = r->rear = 0;
}

/*
 ********************************************************************************************************
 *@func       ring_buf_len
 *@brief      获取环形缓冲区数据长度
 *@param[in]  r - 环形缓冲区管理器
 *@retval     有效字节数
 ********************************************************************************************************
 */
uint32_t ring_buf_len(ring_buffer_t *r)
{
    return r->rear - r->front;
}

/*
 ********************************************************************************************************
 *@func       ring_buf_put
 *@brief      将指定长度的数据放到环形缓冲区中
 *@param[in]  buf - 数据缓冲区
 *            len - 缓冲区长度
 *@retval     实际放到中的数据长度
 ********************************************************************************************************
 */
uint32_t ring_buf_put(ring_buffer_t *r, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t left;
    left = r->size + r->front - r->rear;
    len = min(len, left);
    i = min(len, r->size - (r->rear & r->size - 1));
    memcpy(r->buf + (r->rear & r->size - 1), buf, i);
    memcpy(r->buf, buf + i, len - i);
    r->rear += len;
    return len;
}

/*
 ********************************************************************************************************
 *@func       ring_buf_put
 *@brief      从环形缓冲区中读取指定长度的数据
 *@param[in]  buf - 数据缓冲区
 *            len - 缓冲区长度
 *@retval     实际读取长度
 ********************************************************************************************************
 */
uint32_t ring_buf_get(ring_buffer_t *r, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t left;
    left = r->rear - r->front;
    len = min(len, left);
    i = min(len, r->size - (r->front & r->size - 1));
    memcpy(buf, r->buf + (r->front & r->size - 1), i);
    memcpy(buf + i, r->buf, len - i);
    r->front += len;
    return len;
}