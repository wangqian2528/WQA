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

/*命令项定义*/
typedef struct
{
    char *name;                                  /*命令名*/
    int (*handler)(uint32_t argc, char *argv[]); /*命令处理程序*/
    const char *brief;                           /*命令简介*/
} cli_item_t;

#define __cmd_register(name, handler, brief)               \
    USED ANONY_TYPE(const cli_item_t, __cli_cmd_##handler) \
        SECTION("cli.cmd.1") = {name, handler, brief}

#define cmd_register(name, handler, brief) \
    __cmd_register(name, handler, brief)

void bsp_InitCLI(void);
void bsp_cli_process(void);

#endif // !_BSP_CLI_H