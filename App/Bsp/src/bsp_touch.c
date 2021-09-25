/*
*********************************************************************************************************
*    @Brief   : 电阻屏触摸驱动
*    @Name    : bsp_touch.h
*    @Version : V1.0
*    @Note    : 电阻屏触摸驱动
*    @Logs    :
*    Date            Author       Notes
*    2021-08-19      Walker       Initial version.
*    Copyright (c) 2021, <m18962063673@163.com>
*********************************************************************************************************
*/

#include "bsp.h"

//电阻屏芯片连接引脚
#define PEN                     HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) //T_PEN
#define DOUT                    HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5) //T_MISO
#define TDIN(x)                 HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,x) //T_MOSI
#define TCLK(x)                 HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,x) //T_SCK
#define TCS(x)                  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,x) //T_CS

uint16_t g_touch_x[CT_MAX_TOUCH];
uint16_t g_touch_y[CT_MAX_TOUCH];
uint16_t g_touch_sta;

float g_sys_para_touch_x_fac;//触摸屏X校准参数
float g_sys_para_touch_y_fac;//触摸屏Y校准参数
int16_t g_sys_para_touch_x_off;//触摸屏X校准偏移量
int16_t g_sys_para_touch_y_off;//触摸屏Y校准偏移量
uint8_t g_sys_para_touch_flg;//触摸屏校准标志

/*******************************************************************************
 * @brief       SPI写数据
 * @param[in]   num - 要写入的数据
 * @return      none
 ******************************************************************************/
static void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0;
    for(count = 0; count < 8; count++)
    {
        if(num & 0x80)TDIN(1);
        else TDIN(0);
        num <<= 1;
        TCLK(0);
        bsp_DelayUS(1);
        TCLK(1);//上升沿有效
    }
}

/*******************************************************************************
 * @brief       SPI读数据
 * @param[in]   CMD - 指令
 * @return      读到的数据
 ******************************************************************************/
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
    for(count = 0; count < 16; count++)
    {
        Num <<= 1;
        TCLK(0); //下降沿有效
        bsp_DelayUS(1);
        TCLK(1);
        if(DOUT)Num++;
    }
    Num >>= 4;
    TCS(1);
    return(Num);
}

/*******************************************************************************
 * @brief       读取一个坐标值(x或者y)
                连续读取READ_TIMES次数据,对这些数据升序排列
 * @param[in]   xy - 指令（CMD_RDX/CMD_RDY）
 * @return      读到的数据
 ******************************************************************************/
static uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;
    uint16_t buf[READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    for(i = 0; i < READ_TIMES; i++) buf[i] = TP_Read_AD(xy);
    for(i = 0; i < READ_TIMES - 1; i++) //排序
    {
        for(j = i + 1; j < READ_TIMES; j++)
        {
            if(buf[i] > buf[j]) //升序排列
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for(i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}

/*******************************************************************************
 * @brief       读取x,y坐标
 * @param[out]  x,y:读取到的坐标值
 * @return      返回值:0,失败;1,成功
 ******************************************************************************/
static uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    *x = xtemp;
    *y = ytemp;
    return 1;//读数成功
}

/*******************************************************************************
 * @brief       连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE
 * @param[out]  x,y:读取到的坐标值
 * @return      返回值:0,失败;1,成功
 ******************************************************************************/
static uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;
    uint8_t flag;
    flag = TP_Read_XY(&x1, &y1);
    if(flag == 0)return(0);
    flag = TP_Read_XY(&x2, &y2);
    if(flag == 0)return(0);
    if(((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //前后两次采样在+-50内
            && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else return 0;
}

/*******************************************************************************
 * @brief       与LCD部分有关的函数 画一个红色触摸点用来校准用的
 * @param[in]  x,y:坐标
 * @return      返回值:0,失败;1,成功
 ******************************************************************************/
static void TP_Drow_Touch_Point(uint16_t x, uint16_t y)
{
    bsp_LCD_DrawLine(x - 12, y, x + 13, y, RED); //横线
    bsp_LCD_DrawLine(x, y - 12, x, y + 13, RED); //竖线
    bsp_LCD_DrawPoint(x + 1, y + 1, RED);
    bsp_LCD_DrawPoint(x - 1, y + 1, RED);
    bsp_LCD_DrawPoint(x + 1, y - 1, RED);
    bsp_LCD_DrawPoint(x - 1, y - 1, RED);
    bsp_LCD_DrawCircle(x, y, 6, RED); //画中心圈
}

/*******************************************************************************
 * @brief       触摸按键扫描
 * @param[in]   tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
 * @return      0,触屏无触摸;1,触屏有触摸
 ******************************************************************************/
uint8_t bsp_TouchScan(uint8_t tp)
{
    if(PEN == 0) //有按键按下
    {
        if(tp)TP_Read_XY2(&g_touch_x[0], &g_touch_y[0]); //读取物理坐标
        else if(TP_Read_XY2(&g_touch_x[0], &g_touch_y[0])) //读取屏幕坐标
        {
            g_touch_x[0] = (uint16_t)(g_sys_para_touch_x_fac * g_touch_x[0] + g_sys_para_touch_x_off); //将结果转换为屏幕坐标
            g_touch_y[0] = (uint16_t)(g_sys_para_touch_y_fac * g_touch_y[0] + g_sys_para_touch_y_off);
        }
        
        if((g_touch_sta & TP_PRES_DOWN) == 0) //之前没有被按下
        {
            g_touch_sta = TP_PRES_DOWN | TP_CATH_PRES; //按键按下
            g_touch_x[CT_MAX_TOUCH - 1] = g_touch_x[0]; //记录第一次按下时的坐标
            g_touch_y[CT_MAX_TOUCH - 1] = g_touch_y[0];
            bsp_TouchPutKey(TOUCH_DOWN, g_touch_x[0], g_touch_y[0]);
        }
        else
        {
            bsp_TouchPutKey(TOUCH_MOVE, g_touch_x[0], g_touch_y[0]);
        }
    }
    else
    {
        if(g_touch_sta & TP_PRES_DOWN) //之前是被按下的
        {
            g_touch_sta &= ~TP_PRES_DOWN; //标记按键松开
            bsp_TouchPutKey(TOUCH_RELEASE, g_touch_x[0], g_touch_y[0]);
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

/*******************************************************************************
 * @brief       触摸校准
 * @param[in]   none
 * @return      none
 ******************************************************************************/
void bsp_TouchAdj(void)
{
    uint8_t cnt = 0;
    uint16_t pos_temp[5][2];
    cnt = 0;
    bsp_LCD_Clear(WHITE);
    TP_Drow_Touch_Point(20, 20);
    g_touch_sta = 0;
    while(1)
    {
        bsp_TouchScan(1);
        if((g_touch_sta & 0xc000) == TP_CATH_PRES)
        {
            g_touch_sta &= ~TP_CATH_PRES;
            pos_temp[cnt][0] = g_touch_x[0];
            pos_temp[cnt][1] = g_touch_y[0];
            cnt++;
            switch(cnt)
            {
            case 1:
                bsp_LCD_Clear(WHITE);
                TP_Drow_Touch_Point(LCD_PIX_WIDTH - 20, 20);
                break;
            case 2:
                bsp_LCD_Clear(WHITE);
                TP_Drow_Touch_Point(20, LCD_PIX_HEIGHT - 20);
                break;
            case 3:
                bsp_LCD_Clear(WHITE);
                TP_Drow_Touch_Point(LCD_PIX_WIDTH - 20, LCD_PIX_HEIGHT - 20);
                break;
            case 4:
                g_sys_para_touch_x_fac = (float)(LCD_PIX_WIDTH - 40) / (pos_temp[1][0] - pos_temp[0][0]);
                g_sys_para_touch_x_off = (LCD_PIX_WIDTH - g_sys_para_touch_x_fac * (pos_temp[1][0] + pos_temp[0][0])) / 2;
                g_sys_para_touch_y_fac = (float)(LCD_PIX_HEIGHT - 40) / (pos_temp[2][1] - pos_temp[0][1]);
                g_sys_para_touch_y_off = (LCD_PIX_HEIGHT - g_sys_para_touch_y_fac * (pos_temp[2][1] + pos_temp[0][1])) / 2;
                bsp_LCD_Clear(WHITE);
                TP_Drow_Touch_Point(LCD_PIX_WIDTH / 2, LCD_PIX_HEIGHT / 2);
                break;
            case 5:
                pos_temp[4][0] = (uint16_t)(g_sys_para_touch_x_fac * pos_temp[4][0] + g_sys_para_touch_x_off);
                pos_temp[4][1] = (uint16_t)(g_sys_para_touch_y_fac * pos_temp[4][1] + g_sys_para_touch_y_off);
                if(ABS(pos_temp[4][0], LCD_PIX_WIDTH / 2) > 5 || ABS(pos_temp[4][1], LCD_PIX_HEIGHT / 2) > 5)
                {
                    cnt = 0;
                    bsp_LCD_Clear(WHITE);
                    TP_Drow_Touch_Point(20, 20);
                    continue;
                }
                bsp_LCD_Clear(WHITE);
                return;
            }
        }
    }
}

/*******************************************************************************
 * @brief       读取EEPROM触摸参数
 * @param[in]   none
 * @return      none
 ******************************************************************************/
void bsp_LoadAdj(void)
{
    int temp;
    temp = bsp_EEReadWord(ADDR_SYS_PARA_TOUCH, 4);
    g_sys_para_touch_x_fac = (float)temp / 100000000;
    temp = bsp_EEReadWord(ADDR_SYS_PARA_TOUCH + 4, 4);
    g_sys_para_touch_y_fac = (float)temp / 100000000;
    g_sys_para_touch_x_off = bsp_EEReadWord(ADDR_SYS_PARA_TOUCH + 8, 2);
    g_sys_para_touch_y_off = bsp_EEReadWord(ADDR_SYS_PARA_TOUCH + 10, 2);
    g_sys_para_touch_flg = bsp_EEReadByte(ADDR_SYS_PARA_TOUCH + 12);
}

/*******************************************************************************
 * @brief       存储EEPROM触摸参数
 * @param[in]   none
 * @return      none
 ******************************************************************************/
void bsp_SaveAdj(void)
{
    int temp;
    temp = g_sys_para_touch_x_fac * 100000000;
    bsp_EEWriteWord(ADDR_SYS_PARA_TOUCH, temp, 4);
    temp = g_sys_para_touch_y_fac * 100000000;
    bsp_EEWriteWord(ADDR_SYS_PARA_TOUCH + 4, temp, 4);
    bsp_EEWriteWord(ADDR_SYS_PARA_TOUCH + 8, g_sys_para_touch_x_off, 2);
    bsp_EEWriteWord(ADDR_SYS_PARA_TOUCH + 10, g_sys_para_touch_y_off, 2);
    bsp_EEWriteByte(ADDR_SYS_PARA_TOUCH + 12, g_sys_para_touch_flg);
}

/*
*********************************************************************************************************
 * @brief       bsp_InitTouch
 * @note        初始化Touch
 * @param[in]   none
 * @return      none
*********************************************************************************************************
*/
void bsp_InitTouch(void)
{
    bsp_LoadAdj();
    if(g_sys_para_touch_flg != 0x55)
    {
        g_sys_para_touch_flg = 0x55;
        bsp_TouchAdj();
        bsp_SaveAdj();
    }
}


#include "gx_api.h"
void bsp_TouchPutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
    uint16_t xx, yy;
    GX_EVENT event;

    /* 无需转换， 直接是坐标值 */
    xx = _usX;
    yy = _usY;

    /* 按下, 移动和松手事件 */
    switch (_ucEvent)
    {
        case TOUCH_DOWN:
            event.gx_event_type = GX_EVENT_PEN_DOWN;
            event.gx_event_payload.gx_event_pointdata.gx_point_x = xx;
            event.gx_event_payload.gx_event_pointdata.gx_point_y = yy;
            event.gx_event_sender = 0;
            event.gx_event_target = 0;
            event.gx_event_display_handle = 0x24000000;
            gx_system_event_send(&event);
            break;

        case TOUCH_MOVE:
            event.gx_event_type = GX_EVENT_PEN_DRAG;
            event.gx_event_payload.gx_event_pointdata.gx_point_x = xx;
            event.gx_event_payload.gx_event_pointdata.gx_point_y = yy;
            event.gx_event_sender = 0;
            event.gx_event_target = 0;
            event.gx_event_display_handle = 0x24000000;
            gx_system_event_fold(&event);
            break;

        case TOUCH_RELEASE:
            event.gx_event_type = GX_EVENT_PEN_UP;
            event.gx_event_payload.gx_event_pointdata.gx_point_x = xx;
            event.gx_event_payload.gx_event_pointdata.gx_point_y = yy;
            event.gx_event_sender = 0;
            event.gx_event_target = 0;
            event.gx_event_display_handle = 0x24000000;
            gx_system_event_send(&event);
            break;

        default:
            break;
    }
}
