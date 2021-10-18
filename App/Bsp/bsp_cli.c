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

#include "bsp.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)

static uint8_t rxbuf[CLI_RXBUF_SIZE]; /* 接收缓冲区 */
static ring_buffer_t rbrecv;          /* 缓冲区管理 */

static const cli_item_t cmd_tbl_start SECTION("cli.cmd.0") = {0};
static const cli_item_t cmd_tbl_end SECTION("cli.cmd.2") = {0};

static char recvbuf[CLI_MAX_CMD_LEN + 1]; /*命令接收缓冲区*/
static uint16_t recvcnt;                  /*最大接收长度*/

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
    len = MIN(len, left);
    i = MIN(len, r->size - (r->rear & r->size - 1));
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
    len = MIN(len, left);
    i = MIN(len, r->size - (r->front & r->size - 1));
    memcpy(buf, r->buf + (r->front & r->size - 1), i);
    memcpy(buf + i, r->buf, len - i);
    r->front += len;
    return len;
}

/*
 ********************************************************************************************************
 *@func       bsp_cli_read
 *@brief      读取串口接收缓冲区的数据
 *@param[in]  buf - 数据缓存
 *            len - 数据长度
 *@retval     实际读取长度
 ********************************************************************************************************
 */
uint32_t bsp_cli_read(void *buf, uint32_t len)
{
    return ring_buf_get(&rbrecv, (uint8_t *)buf, len);
}

/*
 ********************************************************************************************************
 *@func       USART3_IRQHandler
 *@brief      USART3中断服务函数
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
void USART3_IRQHandler(void)
{
    uint8_t data;

    if (USART3->ISR & (1 << 5))
    {
        data = USART3->RDR;
        ring_buf_put(&rbrecv, &data, 1); /*将数据放入接收缓冲区*/
    }

    if (USART3->ISR & (1 << 3))
    {
        data = USART3->RDR;
    }
}

/*
 ********************************************************************************************************
 *@func       find_cmd
 *@brief      查找命令
 *@param[in]  keyword - 命令关键字
 *@retval     命令项指针
 ********************************************************************************************************
 */
static const cli_item_t *find_cmd(const char *keyword)
{
    const cli_item_t *it;
    for (it = &cmd_tbl_start + 1; it < &cmd_tbl_end; it++)
    {
        if (!strcasecmp(keyword, it->name))
            return it;
    }
    return NULL;
}

/*
 ********************************************************************************************************
 * @func       strsplit
 * @brief      字符串分割  - 在源字符串查找出所有由separator指定的分隔符
 *                            (如',')并替换成字符串结束符'\0'形成子串，同时令list
 *                            指针列表中的每一个指针分别指向一个子串
 * @example
 *             input=> s = "abc,123,456,,fb$"
 *             separator = ",$"
 *
 *             output=>s = abc'\0' 123'\0' 456'\0' '\0' fb'\0''\0'
 *             list[0] = "abc"
 *             list[1] = "123"
 *             list[2] = "456"
 *             list[3] = ""
 *             list[4] = "fb"
 *             list[5] = ""
 *
 * @param[in] str             - 源字符串
 * @param[in] separator       - 分隔字符串
 * @param[in] list            - 字符串指针列表
 * @param[in] len             - 列表长度
 * @return    list指针列表项数，如上例所示则返回6
 ********************************************************************************************************
 */
static size_t strsplit(char *s, const char *separator, char *list[], size_t len)
{
    size_t count = 0;
    if (s == NULL || list == NULL || len == 0)
        return 0;

    list[count++] = s;
    while (*s && count < len)
    {
        if (strchr(separator, *s) != NULL)
        {
            *s = '\0';
            list[count++] = s + 1; /*指向下一个子串*/
        }
        s++;
    }
    return count;
}

/*
 ********************************************************************************************************
 *@func       process_line
 *@brief      处理行
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
static void process_line(void)
{
    char *argv[CLI_MAX_ARGS];
    uint32_t argc;
    const cli_item_t *it;
    argc = strsplit(recvbuf, " ", argv, CLI_MAX_ARGS);
    if ((it = find_cmd(argv[0])) == NULL)
    {
        App_Printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
        return;
    }
    it->handler(argc, argv);
}

/*
 ********************************************************************************************************
 *@func       bsp_cli_exec
 *@brief      执行一行命令
 *@param[in]  cmd - 命令
 *@retval     none
 ********************************************************************************************************
 */
void bsp_cli_exec(const char *cmd)
{
    snprintf(recvbuf, CLI_MAX_CMD_LEN, "%s", cmd);
    process_line();
}

/*
 ********************************************************************************************************
 *@func       bsp_cli_process
 *@brief      命令行处理程序
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
void bsp_cli_process(void)
{
    uint32_t i;
    i = recvcnt;
    recvcnt += bsp_cli_read(&recvbuf[i], CLI_MAX_CMD_LEN - i);
    while (i < recvcnt)
    {
        if (recvbuf[i] == '\r' || recvbuf[i] == '\n') /*读取1行*/
        {
            recvbuf[i] = '\0';
            process_line();
            recvcnt = 0;
        }
        i++;
    }
}

/*
 ********************************************************************************************************
 *@func       bsp_InitCli
 *@brief      CLI模块初始化
 *@param[in]  none
 *@retval     none
 ********************************************************************************************************
 */
void bsp_InitCLI(void)
{
    ring_buf_init(&rbrecv, rxbuf, sizeof(rxbuf));
    USART3->CR1 |= 1 << 5; //接收缓冲区非空中断使能
}

/*
 ********************************************************************************************************
 *@func       cmd_item_comparer
 *@brief      命令比较器
 *@param[in]  item1 item2
 *@retval     none
 ********************************************************************************************************
 */
static int cmd_item_comparer(const void *item1, const void *item2)
{
    cli_item_t *it1 = *((cli_item_t **)item1);
    cli_item_t *it2 = *((cli_item_t **)item2);
    return strcmp(it1->name, it2->name);
}

/*
 * @brief      帮助命令
 */
static int do_help(uint32_t argc, char *argv[])
{
    uint32_t i, j, count;
    const cli_item_t *item_start = &cmd_tbl_start + 1;
    const cli_item_t *item_end = &cmd_tbl_end;
    const cli_item_t *cmdtbl[CLI_MAX_CMDS];

    if (argc == 2)
    {
        if ((item_start = find_cmd(argv[1])) != NULL)
        {
            App_Printf(item_start->brief); /*命令使用信息----*/
            App_Printf("\r\n");
        }
        return 0;
    }
    for (i = 0; i < item_end - item_start && i < CLI_MAX_ARGS; i++)
        cmdtbl[i] = &item_start[i];
    count = i;
    /*对命令进行排序 ---------------------------------------------------------*/
    qsort(cmdtbl, i, sizeof(cli_item_t *), cmd_item_comparer);
    App_Printf("\r\n");
    App_Printf("===============================================================\r\n");
    App_Printf("---------------------list of all used cmd----------------------\r\n");
    App_Printf("===============================================================\r\n");
    for (i = 0; i < count; i++)
    {
        App_Printf(cmdtbl[i]->name); /*打印命令名------*/
        /*对齐调整*/
        j = strlen(cmdtbl[i]->name);
        if (j < 20)
            j = 20 - j;

        while (j--)
            App_Printf(" ");

        App_Printf("- ");
        App_Printf(cmdtbl[i]->brief); /*命令使用信息----*/
        App_Printf("\r\n");
    }
    return 1;
}

/*注册帮助命令 ---------------------------------------------------------------*/
cmd_register("help", do_help, "list all command");
cmd_register("?", do_help, "alias for 'help'");