/******************************************************************************
 * @brief    LCD模块
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
 * @func        bsp_lcd_draw_point
 * @brief       画点函数
 * @param[in]   x,y:写入坐标
 *              color:颜色值
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    *(uint16_t *)((uint32_t)LCD_LAYER_BUF_ADDR + LCD_PIX_SIZE * (LCD_PIX_WIDTH * y + x)) = color;
}

/*
 ********************************************************************************************************
 * @func        bsp_lcd_draw_line
 * @brief       画点函数
 * @param[in]   x1,y1:起点坐标 x2,y2:终点坐标
 *              color:颜色值
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int16_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int16_t incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1; //设置单步方向
    else if (delta_x == 0)
        incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; //选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++) //画线输出
    {
        bsp_lcd_draw_point(uRow, uCol, color); //画点
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_lcd_draw_circle
 * @brief       在指定位置画一个指定大小的圆
 * @param[in]   (x,y):中心点 r    :半径
 *              color:颜色值
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int16_t a, b;
    int16_t di;
    a = 0;
    b = r;
    di = 3 - (r << 1); //判断下个点位置的标志
    while (a <= b)
    {
        bsp_lcd_draw_point(x0 + a, y0 - b, color); //5
        bsp_lcd_draw_point(x0 + b, y0 - a, color); //0
        bsp_lcd_draw_point(x0 + b, y0 + a, color); //4
        bsp_lcd_draw_point(x0 + a, y0 + b, color); //6
        bsp_lcd_draw_point(x0 - a, y0 + b, color); //1
        bsp_lcd_draw_point(x0 - b, y0 + a, color);
        bsp_lcd_draw_point(x0 - a, y0 - b, color); //2
        bsp_lcd_draw_point(x0 - b, y0 - a, color); //7
        a++;
        //使用Bresenham算法画圆
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/*
 ********************************************************************************************************
 * @func        bsp_lcd_fill
 * @brief       LTDC填充矩形,DMA2D填充
 * @param[in]   (sx,sy),(ex,ey):填充矩形对角坐标,区域大小丿:(ex-sx+1)*(ey-sy+1)
                color:要填充的颜色
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    offline = LCD_PIX_WIDTH - (ex + 1 - sx);
    addr = LCD_LAYER_BUF_ADDR + LCD_PIX_SIZE * (LCD_PIX_WIDTH * sy + sx);
    RCC->AHB3ENR |= 1 << 4;
    DMA2D->CR &= ~(1 << 0);
    DMA2D->CR = 3 << 16;
    DMA2D->OPFCCR = LCD_PIXFORMAT;
    DMA2D->OOR = offline;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (ey - sy + 1) | ((ex - sx + 1) << 16);
    DMA2D->OCOLR = color;
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= 1 << 0;
    while ((DMA2D->ISR & (1 << 1)) == 0)
    {
        timeout++;
        if (timeout > 0X1FFFFF)
            break;
    }
    DMA2D->IFCR |= 1 << 1;
}

/*
 ********************************************************************************************************
 * @func        bsp_lcd_color_fill
 * @brief       LTDC填充填充指定颜色块形,DMA2D填充
 * @param[in]   (sx,sy),(ex,ey):填充矩形对角坐标,区域大小丿:(ex-sx+1)*(ey-sy+1)
                color:要填充的颜色数组首地坿
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    offline = LCD_PIX_WIDTH - (ex + 1 - sx);
    addr = ((uint32_t)LCD_LAYER_BUF_ADDR + LCD_PIX_SIZE * (LCD_PIX_WIDTH * sy + sx));
    RCC->AHB3ENR |= 1 << 4;
    DMA2D->CR &= ~(1 << 0);
    DMA2D->CR = 0 << 16;
    DMA2D->FGPFCCR = LCD_PIXFORMAT;
    DMA2D->FGOR = 0;
    DMA2D->OOR = offline;
    DMA2D->FGMAR = (uint32_t)color;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (ey - sy + 1) | ((ex - sx + 1) << 16);
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= 1 << 0;
    while ((DMA2D->ISR & (1 << 1)) == 0)
    {
        timeout++;
        if (timeout > 0X1FFFFF)
            break;
    }
    DMA2D->IFCR |= 1 << 1;
}

/*
 ********************************************************************************************************
 * @func        bsp_lcd_clear
 * @brief       LCD清屏
 * @param[in]   color:颜色倿
 * @return      none
 ********************************************************************************************************
 */
void bsp_lcd_clear(uint16_t color)
{
    bsp_lcd_fill(0, 0, LCD_PIX_WIDTH - 1, LCD_PIX_HEIGHT - 1, color);
}

/*
 ********************************************************************************************************
 * @func        bsp_InitLCD
 * @brief       LCD模块初始化
 * @param[in]   none
 * @return      none
 ********************************************************************************************************
 */
void bsp_InitLCD(void)
{
    LCD_BK_SET(0);
    LCD_RESET;
    bsp_DelayMS(10);
    LCD_SET;
    bsp_lcd_clear(WHITE);
    // LCD_BK_SET(g_sys_para_lcd_bklight);
}

/*
 * @brief      背光控制cmd
 */
static int do_lcd_bk(uint32_t argc, char *argv[])
{
    if (argc < 2)
        return 0;
    uint8_t bk_temp = atoi(argv[1]);
    __LimitValue(bk_temp, 10, 100);
    g_sys_para_lcd_bklight = bk_temp;
    LCD_BK_SET(g_sys_para_lcd_bklight);
    bsp_eeprom_save(EEPROM_DATA_LCD_BKLIGHT);
    return 1;
}
cmd_register("lcd_bk", do_lcd_bk, "lcd bk light cmd");