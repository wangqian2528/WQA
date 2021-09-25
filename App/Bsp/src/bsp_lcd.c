/*
*********************************************************************************************************
*    @Brief   : LCD驱动
*    @Name    : bsp_lcd.h
*    @Version : V1.0
*    @Note    : LCD驱动
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include "bsp.h"

/*******************************************************************************
 * @brief       画点函数
 * @param[in]   x,y:写入坐标
 * @param[in]   color:颜色值
 * @return      none
 ******************************************************************************/
void bsp_LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    *(uint16_t *)((uint32_t)LCD_FRAME_BUF_ADDR_LAYER + LCD_PIX_SIZE * (LCD_PIX_WIDTH * y + x)) = color;
}

/*******************************************************************************
 * @brief       画线函数
 * @param[in]   x1,y1:起点坐标 x2,y2:终点坐标
 * @return      none
 ******************************************************************************/
void bsp_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int16_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int16_t incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1;
    else if(delta_x == 0)incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x;
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ )
    {
        bsp_LCD_DrawPoint(uRow, uCol, color);
        xerr += delta_x ;
        yerr += delta_y ;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*******************************************************************************
 * @brief       在指定位置画一个指定大小的圆
 * @param[in]   (x,y):中心点 r    :半径
 * @return      none
 ******************************************************************************/
void bsp_LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int16_t a, b;
    int16_t di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while(a <= b)
    {
        bsp_LCD_DrawPoint(x0 + a, y0 - b, color);
        bsp_LCD_DrawPoint(x0 + b, y0 - a, color);
        bsp_LCD_DrawPoint(x0 + b, y0 + a, color);
        bsp_LCD_DrawPoint(x0 + a, y0 + b, color);
        bsp_LCD_DrawPoint(x0 - a, y0 + b, color);
        bsp_LCD_DrawPoint(x0 - b, y0 + a, color);
        bsp_LCD_DrawPoint(x0 - a, y0 - b, color);
        bsp_LCD_DrawPoint(x0 - b, y0 - a, color);
        a++;
        if(di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/*******************************************************************************
 * @brief       LTDC填充矩形,DMA2D填充
 * @param[in]   (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
                color:要填充的颜色
 * @return      none
 ******************************************************************************/
void bsp_LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    offline = LCD_PIX_WIDTH - (ex + 1 - sx);
    addr = LCD_FRAME_BUF_ADDR_LAYER + LCD_PIX_SIZE * (LCD_PIX_WIDTH * sy + sx);
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
    while((DMA2D->ISR & (1 << 1)) == 0)	
    {
        timeout++;
        if(timeout > 0X1FFFFF)break;
    }
    DMA2D->IFCR |= 1 << 1;
}

/*******************************************************************************
 * @brief       LTDC填充填充指定颜色块形,DMA2D填充
 * @param[in]   (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
                color:要填充的颜色数组首地址
 * @return      none
 ******************************************************************************/
void bsp_LCD_ColorFill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    offline = LCD_PIX_WIDTH - (ex  + 1 - sx);
    addr = ((uint32_t)LCD_FRAME_BUF_ADDR_LAYER + LCD_PIX_SIZE * (LCD_PIX_WIDTH * sy + sx));
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
    while((DMA2D->ISR & (1 << 1)) == 0)
    {
        timeout++;
        if(timeout > 0X1FFFFF)break;
    }
    DMA2D->IFCR |= 1 << 1;
}

/*******************************************************************************
 * @brief       LCD清屏
 * @param[in]   color:颜色值
 * @return      none
 ******************************************************************************/
void bsp_LCD_Clear(uint16_t color)
{
    bsp_LCD_Fill(0, 0, LCD_PIX_WIDTH - 1, LCD_PIX_HEIGHT - 1, color);
}
