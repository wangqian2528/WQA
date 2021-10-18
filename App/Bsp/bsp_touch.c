/******************************************************************************
 * @brief    电阻触摸屏模块
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

uint16_t g_touch_x[CT_MAX_TOUCH]; //当前坐标
uint16_t g_touch_y[CT_MAX_TOUCH];
uint16_t g_touch_sta; //笔的状态

/*
 ********************************************************************************************************
 * @func        TP_Write_Byte
 * @brief       SPI写数据
 * @param[in]   num - 要写入的数据
 * @return      none
 ********************************************************************************************************
 */
static void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0;
    for (count = 0; count < 8; count++)
    {
        if (num & 0x80)
            TDIN(1);
        else
            TDIN(0);
        num <<= 1;
        TCLK(0);
        bsp_DelayUS(1);
        TCLK(1); //上升沿有效
    }
}

/*
 ********************************************************************************************************
 * @func        TP_Read_AD
 * @brief       SPI读数据
 * @param[in]   CMD - 指令
 * @return      读到的数据
 ********************************************************************************************************
 */
static uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0;
    uint16_t Num = 0;
    TCLK(0);
    TDIN(0);
    TCS(0);
    TP_Write_Byte(CMD);
    bsp_DelayUS(6);
    TCLK(0);
    bsp_DelayUS(1);
    TCLK(1);
    bsp_DelayUS(1);
    TCLK(0);
    for (count = 0; count < 16; count++)
    {
        Num <<= 1;
        TCLK(0); //下降沿有效
        bsp_DelayUS(1);
        TCLK(1);
        if (DOUT)
            Num++;
    }
    Num >>= 4;
    TCS(1);
    return (Num);
}

/*
 ********************************************************************************************************
 * @func        TP_Read_XOY
 * @brief       读取一个坐标值(x或者y)
 * @param[in]   xy - 指令（CMD_RDX/CMD_RDY）
 * @return      读到的数据
 ********************************************************************************************************
 */
static uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;
    uint16_t buf[READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    for (i = 0; i < READ_TIMES; i++)
        buf[i] = TP_Read_AD(xy);
    for (i = 0; i < READ_TIMES - 1; i++) //排序
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) //升序排列
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
        sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}

/*
 ********************************************************************************************************
 * @func        TP_Read_XY
 * @brief       读取x,y坐标
 * @param[out]  x,y:读取到的坐标值
 * @return      返回值:0,失败;1,成功
 ********************************************************************************************************
 */
static uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    *x = xtemp;
    *y = ytemp;
    return 1; //读数成功
}

/*
 ********************************************************************************************************
 * @func        TP_Read_XY2
 * @brief       连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE
 * @param[out]  x,y:读取到的坐标值
 * @return      返回值:0,失败;1,成功
 ********************************************************************************************************
 */
static uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;
    uint8_t flag;
    flag = TP_Read_XY(&x1, &y1);
    if (flag == 0)
        return (0);
    flag = TP_Read_XY(&x2, &y2);
    if (flag == 0)
        return (0);
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //前后两次采样在+-50内
        && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
        return 0;
}

/*
 ********************************************************************************************************
 * @func        TP_Drow_Touch_Point
 * @brief       与LCD部分有关的函数 画一个红色触摸点用来校准用的
 * @param[in]   x,y:坐标
 * @return      none
 ********************************************************************************************************
 */
static void TP_Drow_Touch_Point(uint16_t x, uint16_t y)
{
    bsp_lcd_draw_line(x - 12, y, x + 13, y, RED); //横线
    bsp_lcd_draw_line(x, y - 12, x, y + 13, RED); //竖线
    bsp_lcd_draw_point(x + 1, y + 1, RED);
    bsp_lcd_draw_point(x - 1, y + 1, RED);
    bsp_lcd_draw_point(x + 1, y - 1, RED);
    bsp_lcd_draw_point(x - 1, y - 1, RED);
    bsp_lcd_draw_circle(x, y, 6, RED); //画中心圈
}

/*
 ********************************************************************************************************
 * @func        bsp_touch_Scan
 * @brief       触摸按键扫描
 * @param[in]   tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
 * @return      0,触屏无触摸;1,触屏有触摸
 ********************************************************************************************************
 */
uint8_t bsp_touch_Scan(uint8_t tp)
{
    if (PEN == 0) //有按键按下
    {
        if (tp)
            TP_Read_XY2(&g_touch_x[0], &g_touch_y[0]);      //读取物理坐标
        else if (TP_Read_XY2(&g_touch_x[0], &g_touch_y[0])) //读取屏幕坐标
        {
            g_touch_x[0] = (uint16_t)(g_sys_para_touch_x_fac * g_touch_x[0] + g_sys_para_touch_x_off); //将结果转换为屏幕坐标
            g_touch_y[0] = (uint16_t)(g_sys_para_touch_y_fac * g_touch_y[0] + g_sys_para_touch_y_off);
        }
        if ((g_touch_sta & TP_PRES_DOWN) == 0) //之前没有被按下
        {
            g_touch_sta = TP_PRES_DOWN | TP_CATH_PRES;  //按键按下
            g_touch_x[CT_MAX_TOUCH - 1] = g_touch_x[0]; //记录第一次按下时的坐标
            g_touch_y[CT_MAX_TOUCH - 1] = g_touch_y[0];
        }
    }
    else
    {
        if (g_touch_sta & TP_PRES_DOWN) //之前是被按下的
        {
            g_touch_sta &= ~TP_PRES_DOWN; //标记按键松开
        }
        else //之前就没有被按下
        {
            g_touch_x[CT_MAX_TOUCH - 1] = 0;
            g_touch_y[CT_MAX_TOUCH - 1] = 0;
            g_touch_x[0] = 0xffff;
            g_touch_y[0] = 0xffff;
        }
    }
    return g_touch_sta & TP_PRES_DOWN; //返回当前的触屏状态
}

/*
 ********************************************************************************************************
 * @func        bsp_touch_adjust
 * @brief       触摸屏校准
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_touch_adjust(void)
{
    uint8_t cnt = 0;
    uint16_t pos_temp[5][2]; //坐标缓存值
    cnt = 0;
    bsp_lcd_clear(WHITE);        //清屏
    TP_Drow_Touch_Point(20, 20); //画点1
    g_touch_sta = 0;             //消除触发信号
    while (1)
    {
        bsp_touch_Scan(1);                          //扫描物理坐标
        if ((g_touch_sta & 0xc000) == TP_CATH_PRES) //按键按下了一次(此时按键松开了.)
        {
            g_touch_sta &= ~TP_CATH_PRES; //标记按键已经被处理过了.
            pos_temp[cnt][0] = g_touch_x[0];
            pos_temp[cnt][1] = g_touch_y[0];
            cnt++;
            switch (cnt)
            {
            case 1:
                bsp_lcd_clear(WHITE);                        //清屏
                TP_Drow_Touch_Point(LCD_PIX_WIDTH - 20, 20); //画点2
                break;
            case 2:
                bsp_lcd_clear(WHITE);                         //清屏
                TP_Drow_Touch_Point(20, LCD_PIX_HEIGHT - 20); //画点3
                break;
            case 3:
                bsp_lcd_clear(WHITE);                                         //清屏
                TP_Drow_Touch_Point(LCD_PIX_WIDTH - 20, LCD_PIX_HEIGHT - 20); //画点4
                break;
            case 4:
                g_sys_para_touch_x_fac = (float)(LCD_PIX_WIDTH - 40) / (pos_temp[1][0] - pos_temp[0][0]);                   //得到xfac
                g_sys_para_touch_x_off = (LCD_PIX_WIDTH - g_sys_para_touch_x_fac * (pos_temp[1][0] + pos_temp[0][0])) / 2;  //得到xoff
                g_sys_para_touch_y_fac = (float)(LCD_PIX_HEIGHT - 40) / (pos_temp[2][1] - pos_temp[0][1]);                  //得到yfac
                g_sys_para_touch_y_off = (LCD_PIX_HEIGHT - g_sys_para_touch_y_fac * (pos_temp[2][1] + pos_temp[0][1])) / 2; //得到yoff
                bsp_lcd_clear(WHITE);                                                                                       //清屏
                TP_Drow_Touch_Point(LCD_PIX_WIDTH / 2, LCD_PIX_HEIGHT / 2);                                                 //画点5
                break;
            case 5:
                pos_temp[4][0] = (uint16_t)(g_sys_para_touch_x_fac * pos_temp[4][0] + g_sys_para_touch_x_off); //将结果转换为屏幕坐标
                pos_temp[4][1] = (uint16_t)(g_sys_para_touch_y_fac * pos_temp[4][1] + g_sys_para_touch_y_off);
                if (ABS(pos_temp[4][0], LCD_PIX_WIDTH / 2) > 5 || ABS(pos_temp[4][1], LCD_PIX_HEIGHT / 2) > 5)
                {
                    cnt = 0;
                    bsp_lcd_clear(WHITE);        //清屏
                    TP_Drow_Touch_Point(20, 20); //画点1
                    continue;
                }
                bsp_lcd_clear(WHITE); //清屏
                return;               //校正完成
            }
        }
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_InitTOUCH
 * @brief       触摸屏初始化
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_InitTOUCH(void)
{
    if (g_sys_para_touch_flg != 0x55)
    {
        bsp_touch_adjust();
        g_sys_para_touch_flg = 0x55;
        bsp_eeprom_save(EEPROM_DATA_TOUCH_ADJ);
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_touch_RunPeriod
 * @brief       触摸检测周期函数
 * @param[in]   period - 周期
 * @return      none
 ********************************************************************************************************
 */
void bsp_touch_RunPeriod(uint32_t period)
{
    static int touch_tick = 0;
    if (touch_tick >= TOUCH_READ_PERIOD)
    {
        touch_tick = 0;
        bsp_touch_Scan(0);
    }
    else
        touch_tick += period;
}

/*
 * @brief      TOUCH校准cmd
 */
static int do_touch_adjust(uint32_t argc, char *argv[])
{
    __disable_irq();
    bsp_touch_adjust();
    g_sys_para_touch_flg = 0x55;
    bsp_eeprom_save(EEPROM_DATA_TOUCH_ADJ);
    __enable_irq();
    return 1;
}
cmd_register("touch_adj", do_touch_adjust, "lcd touch adjust");