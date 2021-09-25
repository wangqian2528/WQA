/*
*********************************************************************************************************
*    @Brief   : 显示驱动
*    @Name    : gx_display_driver_stm32h7_565rgb.c
*    @Version : V1.0
*    @Note    : 底层驱动接口文件
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include   "tx_api.h"
#include   "gx_api.h"
#include   "gx_system.h"
#include   "gx_display.h"
#include   "gx_utility.h"
#include   "gx_display_driver_stm32h7_565rgb.h"

#include   "bsp.h"

/*
*********************************************************************************************************
*                                           变量和函数
*********************************************************************************************************
*/
#define FrameBufer   LCD_FRAME_BUF_ADDR_LAYER   /* 显存地址 */

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_horizontal_line_draw
*    功能说明: 单色线填充
*    形    参: ---
*    返 回 值: 无
*********************************************************************************************************
*/
static VOID gx_chromeart_horizontal_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color)
{
    uint32_t  put;
    int length;
    GX_CANVAS *canvas = context->gx_draw_context_canvas;

    put = (uint32_t) canvas->gx_canvas_memory;
    put += (canvas->gx_canvas_x_resolution * 2 * ypos) + (xstart * 2);

    length = xend - xstart + 1;

    DMA2D->CR = DMA2D_R2M;
    DMA2D->OCOLR = color;

    /* 输出层 */
    DMA2D->OMAR = (uint32_t)put;
    DMA2D->OOR = canvas->gx_canvas_x_resolution - length;
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (uint32_t)(length << 16) | (uint16_t)width;
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START) {}
}

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_vertical_line_draw
*    功能说明: 单色线填充
*    形    参: ---
*    返 回 值: 无
*********************************************************************************************************
*/
static VOID gx_chromeart_vertical_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{
    uint32_t  put;
    GX_CANVAS *canvas = context->gx_draw_context_canvas;
  
    put = (uint32_t) canvas->gx_canvas_memory;

    put += (canvas->gx_canvas_x_resolution * 2 * ystart) + (xpos * 2);

    DMA2D->CR = DMA2D_R2M;
    DMA2D->OCOLR = color;

    /* 输出层 */
    DMA2D->OMAR = (uint32_t)put;
    DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)(yend - ystart + 1);
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START) {}
}

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_canvas_copy
*    功能说明: canvas复制加速
*    形    参: ---
*    返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
    GX_RECTANGLE dirty;
    GX_RECTANGLE overlap;
    USHORT      *read;
    USHORT      *write;
    INT          width;

    dirty.gx_rectangle_left = dirty.gx_rectangle_top = 0;
    dirty.gx_rectangle_right = canvas->gx_canvas_x_resolution - 1;
    dirty.gx_rectangle_bottom = canvas->gx_canvas_y_resolution - 1;

    _gx_utility_rectangle_shift(&dirty, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite ->gx_canvas_dirty_area, &overlap))
    {
        width = overlap.gx_rectangle_right - overlap.gx_rectangle_left + 1;
        read = (USHORT *)canvas->gx_canvas_memory;

        read += (overlap.gx_rectangle_top - dirty.gx_rectangle_top) * canvas->gx_canvas_x_resolution;
        read += overlap.gx_rectangle_left - dirty.gx_rectangle_left;

        write = (USHORT *)composite->gx_canvas_memory;
        write += overlap.gx_rectangle_top * composite->gx_canvas_x_resolution;
        write += overlap.gx_rectangle_left;

        /* 前景层 */
        DMA2D->CR = DMA2D_M2M;
        DMA2D->FGMAR = (uint32_t) read;
        DMA2D->FGOR = canvas->gx_canvas_x_resolution - width; 
        DMA2D->FGPFCCR = (0xff000000) | LTDC_PIXEL_FORMAT_RGB565;
    
        /* 输出层 */
        DMA2D->OMAR = (uint32_t)write;
        DMA2D->OOR = LCD_PIX_WIDTH - width;
        DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
        
        DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)(overlap.gx_rectangle_bottom - overlap.gx_rectangle_top + 1);
        SCB_CleanInvalidateDCache();
        DMA2D->CR |= DMA2D_CR_START;
        while (DMA2D->CR & DMA2D_CR_START) {}
    }
}

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_pixelmap_draw
*    功能说明: pixelmap 绘制加速 
*    形    参: --
*    返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_pixelmap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
    INT           width;
    INT           height;
    USHORT       *putrow;
    USHORT       *getshort;
    ULONG        *getlong;
    GX_CANVAS    *canvas;
    ULONG         reg_val;
    uint32_t      DMA2D_Mode;

    /* 如果pixelmap是压缩的，采用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        _gx_display_driver_565rgb_pixelmap_draw(context, xpos, ypos, pixelmap);
        return;
    }

    /* 如果是带alpha通道的alpha格式，也需要用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA &&
        pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        _gx_display_driver_565rgb_pixelmap_draw(context, xpos, ypos, pixelmap);
        return;
    }

    /* 如果是pixelmap原始数据，使用DMA2D加速 */
    GX_RECTANGLE *clip = context->gx_draw_context_clip;
    canvas = context->gx_draw_context_canvas;

    putrow = (USHORT *)context->gx_draw_context_memory;
    putrow += clip->gx_rectangle_top * context->gx_draw_context_pitch;
    putrow += clip->gx_rectangle_left;
    
    
    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        getshort = (USHORT *)(pixelmap->gx_pixelmap_data);
        getshort += pixelmap->gx_pixelmap_width * (clip->gx_rectangle_top - ypos);
        getshort += (clip->gx_rectangle_left - xpos);
        DMA2D->FGMAR = (uint32_t) getshort;           
    }
    else
    {
        getlong = (ULONG *)(pixelmap ->gx_pixelmap_data);
        getlong += pixelmap ->gx_pixelmap_width * (clip ->gx_rectangle_top - ypos);
        getlong += (clip ->gx_rectangle_left - xpos);
        DMA2D->FGMAR = (uint32_t) getlong;             
    }

    width = clip->gx_rectangle_right - clip ->gx_rectangle_left + 1;
    height = clip->gx_rectangle_bottom - clip->gx_rectangle_top + 1;

    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        reg_val = LTDC_PIXEL_FORMAT_RGB565;
    }
    else
    {
        reg_val = LTDC_PIXEL_FORMAT_ARGB8888;
    }
    
    /* 前景层 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        reg_val |= 0xff000000;
        DMA2D->FGPFCCR = reg_val;
        DMA2D_Mode = DMA2D_M2M_BLEND;
    }
    else
    {
        reg_val |= 0xff010000;  
        DMA2D->FGPFCCR = reg_val;
        DMA2D_Mode = DMA2D_M2M;
    }
    
    /* 前景层 */
    DMA2D->CR = DMA2D_Mode;;
    DMA2D->FGOR = pixelmap->gx_pixelmap_width - width;

    /* 背景层 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        DMA2D->BGMAR = (uint32_t) putrow;
        DMA2D->BGOR = canvas->gx_canvas_x_resolution - width; 
        DMA2D->BGPFCCR = 0xff010002;
    }


    /* 输出层 */
    DMA2D->OMAR = (uint32_t)putrow;
    DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START) {}
}

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_pixelmap_blend
*    功能说明: pixelmap blend 加速
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_pixelmap_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, UCHAR alpha)
{
    INT           width;
    INT           height;
    USHORT       *putrow;
    USHORT       *getshort;
    ULONG        *getlong;
    GX_CANVAS    *canvas;
    ULONG        blend_reg;

     /* 如果pixelmap是压缩的，采用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        _gx_display_driver_565rgb_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        return;
    }
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA &&
        pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        _gx_display_driver_565rgb_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        return;
    }

    /* 使用DMA2D加速绘制 */
    GX_RECTANGLE *clip = context ->gx_draw_context_clip;
    canvas = context->gx_draw_context_canvas;

    putrow = (USHORT *)context ->gx_draw_context_memory;
    putrow += clip->gx_rectangle_top * context->gx_draw_context_pitch;
    putrow += clip->gx_rectangle_left;

    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        getshort = (USHORT *)(pixelmap ->gx_pixelmap_data);
        getshort += pixelmap ->gx_pixelmap_width * (clip ->gx_rectangle_top - ypos);
        getshort += (clip ->gx_rectangle_left - xpos);
    }
    else
    {
        getlong = (ULONG *)(pixelmap ->gx_pixelmap_data);
        getlong += pixelmap ->gx_pixelmap_width * (clip ->gx_rectangle_top - ypos);
        getlong += (clip ->gx_rectangle_left - xpos);
    }

    width = clip->gx_rectangle_right - clip ->gx_rectangle_left + 1;
    height = clip->gx_rectangle_bottom - clip->gx_rectangle_top + 1;
    
    blend_reg = alpha;
    blend_reg <<= 24;
    
    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        DMA2D->FGMAR = (uint32_t) getshort;             
        blend_reg |= LTDC_PIXEL_FORMAT_RGB565;
    }
    else
    {
        DMA2D->FGMAR = (uint32_t) getlong;             
        blend_reg |= LTDC_PIXEL_FORMAT_ARGB8888;
    }

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        blend_reg |= 0x00020000;
    }
    else
    {
        blend_reg |= 0x00010000;
    }

    DMA2D->CR = 0x00020000UL | (1 << 9);

    /* 前景层 */
    DMA2D->FGOR = pixelmap->gx_pixelmap_width - width; 
    DMA2D->FGPFCCR = blend_reg;  
    
    /* 背景层 */
    DMA2D->BGMAR = (uint32_t) putrow;
    DMA2D->BGOR = canvas->gx_canvas_x_resolution - width; 
    DMA2D->BGPFCCR = 0xff010002;

    /* 输出层 */
    DMA2D->OMAR = (uint32_t)putrow;
    DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START) {}
}

/*
*********************************************************************************************************
*    函 数 名: gx_chromeart_glyph_8bit_draw
*    功能说明: glyph 8bit 加速
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area, GX_POINT *map_offset, const GX_GLYPH *glyph)
{
    GX_CANVAS  *canvas;
    GX_UBYTE   *read_glyph;
    USHORT     *write_row;
    GX_COLOR    text_color;
    UINT        height;
    UINT        width;
    UCHAR       red, green, blue;
    GX_UBYTE    alpha;

    text_color =  context->gx_draw_context_brush.gx_brush_line_color;

    alpha = context ->gx_draw_context_brush.gx_brush_alpha;

    if (alpha == 0)
    {
        return;
    }

    red = (text_color & 0xf800) >> 8;
    green = (text_color & 0x07e0) >> 3;
    blue = (text_color & 0x001f) << 3;
    text_color = (red << 16) | (green << 8) | blue;
    width = draw_area->gx_rectangle_right - draw_area->gx_rectangle_left + 1;

    canvas = context->gx_draw_context_canvas;

    read_glyph = (GX_UBYTE *)glyph->gx_glyph_map;

    if (map_offset->gx_point_y)
    {
        read_glyph = read_glyph + (glyph->gx_glyph_width * map_offset->gx_point_y);
    }

    read_glyph += map_offset->gx_point_x;

    width = draw_area->gx_rectangle_right - draw_area->gx_rectangle_left + 1;
    height = draw_area->gx_rectangle_bottom - draw_area->gx_rectangle_top + 1;

    write_row = (USHORT *) canvas->gx_canvas_memory;
    write_row += draw_area->gx_rectangle_top * context->gx_draw_context_pitch;
    write_row += draw_area->gx_rectangle_left;
   
    DMA2D->CR = DMA2D_M2M_BLEND;

    /* 前景层 */
    DMA2D->FGMAR = (uint32_t)read_glyph;
    DMA2D->FGOR = glyph->gx_glyph_width - width; 
    DMA2D->FGCOLR = text_color;
    DMA2D->FGPFCCR =(alpha << 24) | 0x00020009;   /* 原始前景层图像的 alpha 
    通道值替换为当前配置的与原始 alpha 通道值的乘积，AL88 */

    /* 背景层 */
    DMA2D->BGMAR = (uint32_t) write_row;         
    DMA2D->BGOR = canvas->gx_canvas_x_resolution - width;  
    DMA2D->BGPFCCR = 0xFF000002;                  /* 不修改背景层图像的 alpha 通道值，RGB565 */
    
    /* 输出层 */
    DMA2D->OMAR = (uint32_t)write_row;
    DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;
    SCB_CleanInvalidateDCache();
    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START) {}
}

/*
*********************************************************************************************************
*    函 数 名: stm32h7_565rgb_buffer_toggle
*    功能说明: 更新canvas内容到LCD显存
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void stm32h7_565rgb_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    GX_RECTANGLE    Limit;
    GX_RECTANGLE    Copy;
    ULONG           offset;
    INT             copy_width;
    INT             copy_height;

    ULONG *get;
    ULONG *put;

    gx_utility_rectangle_define(&Limit, 0, 0,
                        canvas->gx_canvas_x_resolution - 1,
                        canvas->gx_canvas_y_resolution - 1);

    if (gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        Copy.gx_rectangle_left &= 0xfffe;
        Copy.gx_rectangle_right |= 0x01;
        copy_width = Copy.gx_rectangle_right - Copy.gx_rectangle_left + 1;
        copy_height = Copy.gx_rectangle_bottom - Copy.gx_rectangle_top + 1;

        /* 从canvas读取更新区 */
        offset = Copy.gx_rectangle_top * canvas->gx_canvas_x_resolution;
        offset += Copy.gx_rectangle_left;
        offset /= 2; 
        get = canvas ->gx_canvas_memory;
        get += offset;

        /* 从LCD显存读取要更新的区域，将canvas更新的数据复制进来 */
        put = (ULONG *) FrameBufer;
        offset = (canvas->gx_canvas_display_offset_y + Copy.gx_rectangle_top)* LCD_PIX_WIDTH;
        offset += canvas->gx_canvas_display_offset_x + Copy.gx_rectangle_left;
        offset /= 2;
        put += offset;

        DMA2D->CR = 0x00000000UL | (1 << 9);
        DMA2D->FGMAR = (uint32_t)get;
        DMA2D->OMAR = (uint32_t)put;
        DMA2D->FGOR = canvas->gx_canvas_x_resolution - copy_width;
        DMA2D->OOR = LCD_PIX_WIDTH - copy_width;

        /* 前景层和输出区域都采用RGB565颜色格式 */
        DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
        DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
        DMA2D->NLR = (uint32_t)(copy_width << 16) | (uint16_t)copy_height;
        DMA2D->CR |= DMA2D_CR_START;
        while (DMA2D->CR & DMA2D_CR_START) {}
    }
}

/*
*********************************************************************************************************
*    函 数 名: stm32h7_graphics_driver_setup_565rgb
*    功能说明: 驱动接口函数
*    形    参: ---
*    返 回 值: GX_SUCCESS
*********************************************************************************************************
*/
UINT stm32h7_graphics_driver_setup_565rgb(GX_DISPLAY *display)
{
    _gx_display_driver_565rgb_setup(display, (VOID*)STM32_SCREEN_HANDLE, stm32h7_565rgb_buffer_toggle);
    display->gx_display_driver_pixelmap_blend     = gx_chromeart_pixelmap_blend;
    display->gx_display_driver_pixelmap_draw      = gx_chromeart_pixelmap_draw;
    display->gx_display_driver_canvas_copy        = gx_chromeart_canvas_copy;
    display->gx_display_driver_horizontal_line_draw = gx_chromeart_horizontal_line_draw;
    display->gx_display_driver_vertical_line_draw = gx_chromeart_vertical_line_draw;
    display->gx_display_driver_8bit_glyph_draw    = gx_chromeart_glyph_8bit_draw;
    return(GX_SUCCESS);
}
