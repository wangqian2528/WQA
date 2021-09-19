#include "sys.h"

void GPIO_AF_Set(GPIO_TypeDef *GPIOx, u8 BITx, u8 AFx)
{
    GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
    GPIOx->AFR[BITx >> 3] |= (u32)AFx << ((BITx & 0X07) * 4);
}

void GPIO_Set(GPIO_TypeDef *GPIOx, u32 BITx, u32 MODE, u32 OTYPE, u32 OSPEED, u32 PUPD)
{
    u32 pinpos = 0, pos = 0, curpin = 0;
    for(pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos; //一个个位检查
        curpin = BITx & pos; //检查引脚是否要设置
        if(curpin == pos) //需要设置
        {
            GPIOx->MODER &= ~(3 << (pinpos * 2)); //先清除原来的设置
            GPIOx->MODER |= MODE << (pinpos * 2); //设置新的模式
            if((MODE == 0X01) || (MODE == 0X02)) //如果是输出模式/复用功能模式
            {
                GPIOx->OSPEEDR &= ~(3 << (pinpos * 2)); //清除原来的设置
                GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); //设置新的速度值
                GPIOx->OTYPER &= ~(1 << pinpos) ;   //清除原来的设置
                GPIOx->OTYPER |= OTYPE << pinpos;   //设置新的输出模式
            }
            GPIOx->PUPDR &= ~(3 << (pinpos * 2)); //先清除原来的设置
            GPIOx->PUPDR |= PUPD << (pinpos * 2); //设置新的上下拉
        }
    }
}

void GPIO_Pin_Set(GPIO_TypeDef *GPIOx, u16 pinx, u8 status)
{
    if(status & 0X01)GPIOx->BSRRL = pinx; //设置GPIOx的pinx为1
    else GPIOx->BSRRH = pinx;           //设置GPIOx的pinx为0
}

u8 GPIO_Pin_Get(GPIO_TypeDef *GPIOx, u16 pinx)
{
    if(GPIOx->IDR & pinx)return 1;      //pinx的状态为1
    else return 0;                      //pinx的状态为0
}

//使能STM32H7的L1-Cache,同时开启D cache的强制透写
void Cache_Enable(void)
{
    SCB_EnableICache();   //使能I-Cache,函数在core_cm7.h里面定义
    SCB_EnableDCache();   //使能D-Cache,函数在core_cm7.h里面定义
    SCB->CACR |= 1 << 2; //强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题
}

u8 Sys_Clock_Set(u32 plln, u32 pllm, u32 pllp, u32 pllq)
{
    u16 retry = 0;
    u8 status = 0;

    PWR->CR3 &= ~(1 << 2);          //SCUEN=0,锁定LDOEN和BYPASS位的设置
    PWR->D3CR |= 3 << 14;           //VOS=3,Scale1,1.15~1.26V内核电压,FLASH访问可以得到最高性能
    while((PWR->D3CR & (1 << 13)) == 0); //等待电压稳定

    RCC->CR |= 1 << 0;          //开启HSI
    RCC->ICSCR &= ~(32 << 12);  //HSI校准值清零
    RCC->ICSCR |= 16 << 12; //HSI校准值为16
    while((RCC->CR & (1 << 2)) == 0); //等待HSI启动稳定

    RCC->CR |= 3 << 3;          //HSIDIV[1:0]=3,HSI输出8MHz
    RCC->PLLCKSELR |= 0 << 0;   //PLLSRC[1:0]=0,选择HSI作为PLL的输入时钟源
    RCC->PLLCKSELR |= pllm << 4; //DIVM1[5:0]=pllm,设置PLL1的预分频系数
    RCC->PLL1DIVR &= ~((129 - 1) << 0); //首先清零plln
    RCC->PLL1DIVR |= (plln - 1) << 0; //DIVN1[8:0]=plln-1,设置PLL1的倍频系数,设置值需减1
    RCC->PLL1DIVR |= (pllp - 1) << 9; //DIVP1[6:0]=pllp-1,设置PLL1的p分频系数,设置值需减1
    RCC->PLL1DIVR |= (pllq - 1) << 16; //DIVQ1[6:0]=pllq-1,设置PLL1的q分频系数,设置值需减1
    RCC->PLL1DIVR |= 1 << 24;   //DIVR1[6:0]=pllr-1,设置PLL1的r分频系数,设置值需减1,r分频出来的时钟没用到
    RCC->PLLCFGR |= 2 << 2;     //PLL1RGE[1:0]=2,PLL1输入时钟频率在4~8Mhz之间(25/5=5Mhz),如修改pllm,请确认此参数
    RCC->PLLCFGR |= 0 << 1;     //PLL1VCOSEL=0,PLL1宽的VCO范围,192~836Mhz
    RCC->PLLCFGR |= 3 << 16;    //DIVP1EN=1,DIVQ1EN=1,使能pll1_p_ck和pll1_q_ck
    RCC->CR |= 1 << 24;         //PLL1ON=1,使能PLL1
    while((RCC->CR & (1 << 25)) == 0); //PLL1RDY=1?,等待PLL1准备好

    RCC->D1CFGR |= 8 << 0;      //HREF[3:0]=8,rcc_hclk1/2/3/4=sys_d1cpre_ck/2=400/2=200Mhz,即AHB1/2/3/4=200Mhz
    RCC->D1CFGR |= 0 << 8;      //D1CPRE[2:0]=0,sys_d1cpre_ck=sys_clk/1=400/1=400Mhz,即CPU时钟=400Mhz
    RCC->CFGR |= 3 << 0;        //SW[2:0]=3,系统时钟(sys_clk)选择来自pll1_p_ck,即400Mhz
    while(1)
    {
        retry = (RCC->CFGR & (7 << 3)) >> 3; //获取SWS[2:0]的状态,判断是否切换成功
        if(retry == 3)break;    //成功将系统时钟源切换为pll1_p_ck
    }

    FLASH->ACR |= 2 << 0;       //LATENCY[2:0]=2,2个CPU等待周期(@VOS1 Level,maxclock=210Mhz)
    FLASH->ACR |= 2 << 4;       //WRHIGHFREQ[1:0]=2,flash访问频率<285Mhz

    RCC->D1CFGR |= 4 << 4;      //D1PPRE[2:0]=4,rcc_pclk3=rcc_hclk3/2=100Mhz,即APB3=100Mhz
    RCC->D2CFGR |= 4 << 4;      //D2PPRE1[2:0]=4,rcc_pclk1=rcc_hclk1/2=100Mhz,即APB1=100Mhz
    RCC->D2CFGR |= 4 << 8;      //D2PPRE2[2:0]=4,rcc_pclk2=rcc_hclk1/2=100Mhz,即APB2=100Mhz
    RCC->D3CFGR |= 4 << 4;      //D3PPRE[2:0]=4,rcc_pclk4=rcc_hclk4/2=100Mhz,即APB4=100Mhz

    RCC->CR |= 1 << 7;          //CSION=1,使能CSI,为IO补偿单元提供时钟
    RCC->APB4ENR |= 1 << 1;     //SYSCFGEN=1,使能SYSCFG时钟
    SYSCFG->CCCSR |= 1 << 0;    //EN=1,使能IO补偿单元

    return status;
}

void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq)
{
    RCC->CR = 0x00000001;           //设置HSION,开启内部高速RC振荡，其他位全清零
    RCC->CFGR = 0x00000000;         //CFGR清零
    RCC->D1CFGR = 0x00000000;       //D1CFGR清零
    RCC->D2CFGR = 0x00000000;       //D2CFGR清零
    RCC->D3CFGR = 0x00000000;       //D3CFGR清零
    RCC->PLLCKSELR = 0x00000000;    //PLLCKSELR清零
    RCC->PLLCFGR = 0x00000000;      //PLLCFGR清零
    RCC->CIER = 0x00000000;         //CIER清零,禁止所有RCC相关中断
    //AXI_TARG7_FN_MOD寄存器,由于没有在stm32h743xx.h里面定义,所以,只能用直接
    //操作地址的方式,来修改,该寄存器在<<STM32H7xx参考手册>>第212页,table 23
    *((vu32 *)0x51008108) = 0x00000001; //设置AXI SRAM的矩阵读取能力为1
    Cache_Enable();                 //使能L1 Cache
    Sys_Clock_Set(plln, pllm, pllp, pllq); //设置时钟
}
