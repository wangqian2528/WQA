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

#ifndef _BSP_LCD_H
#define _BSP_LCD_H

/*
 *@brief     颜色像素格式
 */
#define LCD_PIXFORMAT_ARGB8888 0x00
#define LCD_PIXFORMAT_RGB888 0x01
#define LCD_PIXFORMAT_RGB565 0x02
#define LCD_PIXFORMAT_ARGB1555 0x03
#define LCD_PIXFORMAT_ARGB4444 0x04
#define LCD_PIXFORMAT_L8 0x05
#define LCD_PIXFORMAT_AL44 0x06
#define LCD_PIXFORMAT_AL88 0x07

/*
 *@brief     RGB565颜色
 */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 //棕色
#define BRRED 0XFC07 //棕红色
#define GRAY 0X8430  //灰色

#define LCD_PIX_SIZE 2                     //像素点byte大小 RGB565 16BIT
#define LCD_PIX_WIDTH 320                  //屏幕像素宽度
#define LCD_PIX_HEIGHT 240                 //屏幕像素高度
#define LCD_PIXFORMAT LCD_PIXFORMAT_RGB565 //定义颜色像素格式,RGB565
#define LCD_LAYER_BUF_SIZE 153600          //byte 定义单层缓存大小
#define LCD_LAYER_BUF_ADDR 0x24000000      //LCD帧缓冲区首地址,定义在AXI SRAM

//LCD复位
#define LCD_RESET HAL_GPIO_WritePin(LTDC_RST_GPIO_Port, LTDC_RST_Pin, 0)
#define LCD_SET HAL_GPIO_WritePin(LTDC_RST_GPIO_Port, LTDC_RST_Pin, 1)

//LCD背光控制
#define LCD_BK_SET(x)                                     \
    do                                                    \
    {                                                     \
        __HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, x); \
        HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);        \
    } while (0)

void bsp_InitLCD(void);
void bsp_lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void bsp_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void bsp_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void bsp_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void bsp_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void bsp_lcd_clear(uint16_t color);

#endif // !_BSP_LCD_H