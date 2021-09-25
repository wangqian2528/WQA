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

#ifndef _BSP_LCD_H
#define _BSP_LCD_H

/*
 *@brief     RGB565颜色
 */
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色

/*
 *@brief     颜色像素格式
 */
#define LCD_PIXFORMAT_ARGB8888          0x00
#define LCD_PIXFORMAT_RGB888            0x01
#define LCD_PIXFORMAT_RGB565            0x02
#define LCD_PIXFORMAT_ARGB1555          0x03
#define LCD_PIXFORMAT_ARGB4444          0x04
#define LCD_PIXFORMAT_L8                0x05
#define LCD_PIXFORMAT_AL44              0x06
#define LCD_PIXFORMAT_AL88              0x07

//定义颜色像素格式,RGB565
#define LCD_PIXFORMAT                   LCD_PIXFORMAT_RGB565
//定义默认背景层颜色
#define LCD_BACKLAYER_COLOR             0
//定义单层缓存大小
#define LCD_LAYER_BUF_SIZE              153600//byte
//LCD帧缓冲区首地址,这里定义在AXI SRAM
#define LCD_FRAME_BUF_ADDR_LAYER        0x24000000

#define LCD_PIX_WIDTH                   320
#define LCD_PIX_HEIGHT                  240
#define LCD_PIX_SIZE                    2

void bsp_LCD_DrawPoint      (uint16_t x, uint16_t y, uint16_t color);
void bsp_LCD_DrawLine       (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void bsp_LCD_DrawCircle     (uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void bsp_LCD_Fill           (uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void bsp_LCD_ColorFill      (uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void bsp_LCD_Clear          (uint16_t color);

#endif
