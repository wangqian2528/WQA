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

#ifndef _BSP_RING_BUFFER_H
#define _BSP_RING_BUFFER_H

/*环形缓冲区管理器*/
typedef struct
{
    uint8_t *buf;   /*环形缓冲区        */
    uint32_t size;  /*环形缓冲区        */
    uint32_t front; /*头指针          */
    uint32_t rear;  /*尾指针          */
} ring_buffer_t;

bool ring_buf_init(ring_buffer_t *r, uint8_t *buf, uint32_t len);
void ring_buf_clr(ring_buffer_t *r);
uint32_t ring_buf_len(ring_buffer_t *r);
uint32_t ring_buf_put(ring_buffer_t *r, uint8_t *buf, uint32_t len);
uint32_t ring_buf_get(ring_buffer_t *r, uint8_t *buf, uint32_t len);

#endif // !_BSP_RING_BUFFER_H