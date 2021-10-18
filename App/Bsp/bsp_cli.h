/******************************************************************************
 * @brief    命令行CLI模块
 *
 * Copyright (c) 2021, <m18962063673@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-20     walker       Initial version.
 ******************************************************************************/

#ifndef _BSP_CLI_H
#define _BSP_CLI_H

#define CLI_RXBUF_SIZE 256

#define CLI_MAX_CMD_LEN 64 /*命令行长度*/
#define CLI_MAX_ARGS 16    /*最大参数个数*/
#define CLI_MAX_CMDS 64    /*最大允许定义的命令个数*/

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

/*命令项定义*/
typedef struct
{
    char *name;                                  /*命令名*/
    int (*handler)(uint32_t argc, char *argv[]); /*命令处理程序*/
    const char *brief;                           /*命令简介*/
} cli_item_t;

#define ANONY_CONN(type, var, line) type var##line
#define ANONY_DEF(type, prefix, line) ANONY_CONN(type, prefix, line)
#define ANONY_TYPE(type, prefix) ANONY_DEF(type, prefix, __LINE__)
#define SECTION(x) @x
#define USED __root

#define __cmd_register(name, handler, brief)                \
    USED ANONY_TYPE(const cli_item_t, __cli_cmd_##handler) \
        SECTION("cli.cmd.1") = {name, handler, brief}

#define cmd_register(name, handler, brief) \
    __cmd_register(name, handler, brief)

void bsp_InitCLI(void);
void bsp_cli_process(void);

#endif // !_BSP_CLI_H