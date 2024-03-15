---
title: STM32学习笔记（九）IC
date: 2022/1/10
---
&#8195;&#8195;输入捕获模式可以用来测量脉冲宽度或者测量频率。STM32的定时器，除了`TIM6`和`TIM7`，其他定时器都有输入捕获功能。STM32的输入捕获，简单的说就是通过检测`TIMx_CHx`上的边沿信号，在边沿信号发生跳变（比如上升沿/下降沿）的时候，将当前定时器的值`TIMx_CNT`存放到对应的通道的捕获/比较寄存器`TIMx_CCRx`里面，完成一次捕获。同时还可以配置捕获时是否触发中断DMA等。
# 计时器配置
&#8195;&#8195;这部分前面已经介绍过，这里不再展开。
# 输入捕获配置
&#8195;&#8195;库函数是通过`void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct)`函数来初始化输入比较参数的，参数结构体`TIM_ICInitTypeDef`定义如下：
```
typedef struct { 
    uint16_t TIM_Channel; 
    uint16_t TIM_ICPolarity; 
    uint16_t TIM_ICSelection; 
    uint16_t TIM_ICPrescaler; 
    uint16_t TIM_ICFilter; 
} TIM_ICInitTypeDef;
```
&#8195;&#8195;第一个参数`TIM_Channel`用来设置通道，取值如下：
```
#define TIM_Channel_1                      ((uint16_t)0x0000)
#define TIM_Channel_2                      ((uint16_t)0x0004)
#define TIM_Channel_3                      ((uint16_t)0x0008)
#define TIM_Channel_4                      ((uint16_t)0x000C)
```
&#8195;&#8195;第二个参数`TIM_ICPolarity`用来设置输入信号的有效捕获极性，例如设置通道1捕获极性的方法为：
```
TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling)；
```
&#8195;&#8195;第三个参数`TIM_ICSelection`用来设置映射关系，取值如下：
```
#define TIM_ICSelection_DirectTI           ((uint16_t)0x0001)
// TIM Input 1, 2, 3 or 4 is selected to be connected to IC1, IC2, IC3 or IC4, respectively. 
#define TIM_ICSelection_IndirectTI         ((uint16_t)0x0002)
// TIM Input 1, 2, 3 or 4 is selected to be connected to IC2, IC1, IC4 or IC3, respectively. 
#define TIM_ICSelection_TRC                ((uint16_t)0x0003) 
// TIM Input 1, 2, 3 or 4 is selected to be connected to TRC. 
```
&#8195;&#8195;第四个参数`TIM_ICPrescaler`用来设置输入捕获分频系数。
&#8195;&#8195;第五个参数`TIM_ICFilter`设置滤波器长度。
&#8195;&#8195;配置计时器5通道1输入捕获的方法是：
```
TIM_ICInitTypeDef TIM5_ICInitStructure; 
TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //选择输入端 IC1 映射到 TI1 上 
TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //上升沿捕获 
TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到 TI1 上 
TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //配置输入分频,不分频 
TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波 
TIM_ICInit(TIM5, &TIM5_ICInitStructure); 
```
&#8195;&#8195;使能定时器的捕获和更新中断的方法是：
```
TIM_ITConfig( TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断和捕获中断
```
&#8195;&#8195;前者在计时器到时触发，后者在捕获时触发。至于中断优先级配置，只需注意`NVIC_InitStructure.NVIC_IRQChannel`参数的配置。
&#8195;&#8195;其它中断配置和计时器使能事项参考通用计时器部分。

&#8195;&#8195;综上，输入捕获的使用方法：
1. 计时器初始化。
2. 配置计时器输入捕获。
3. 配置中断，编写中断处理函数。
4. 使能计时器。


**参考：正点原子STM32F1开发指南-库函数版本_V3.3、STM32中文参考手册_V10**

---
示例：
```
/*
 * 本实验利用TIM5_CH1来做输入捕获，我们将捕获PA0上的高电平脉宽，并将脉宽时间通过串
 * 口打印出来，大家可以通过按WK_UP按键，模拟输入高电平。
 */

#include <stdio.h>
#include "stm32f10x.h"

/* 加入以下代码,支持printf函数,而不需要选择use MicroLIB */

/* 标准库需要的支持函数 */
struct __FILE
{
	int handle;
};

FILE __stdout;
/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit( int x )
{
	x = x;
}


/* 重定义fputc函数 */
int fputc( int ch, FILE *f )
{
	while ( (USART1->SR & 0X40) == 0 )
		;  /* 循环发送,直到发送完毕 */
	USART1->DR = (u8) ch;
	return(ch);
}


void uart_init( u32 bound )
{
	/* GPIO端口设置 */
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );         /* 使能USART1，GPIOA时钟 */

	/* USART1_TX   GPIOA.9 */
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_9;                                           /* PA.9 */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                                      /*复用推挽输出 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                                /* 初始化GPIOA.9 */

	/* USART1_RX	  GPIOA.10初始化 */
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_10;                                          /* PA10 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;                                /* 浮空输入 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                                /* 初始化GPIOA.10 */

	/* USART 初始化设置 */
	USART_InitStructure.USART_BaudRate		= bound;                                /* 串口波特率 */
	USART_InitStructure.USART_WordLength		= USART_WordLength_8b;                  /* 字长为8位数据格式 */
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;                     /* 一个停止位 */
	USART_InitStructure.USART_Parity		= USART_Parity_No;                      /* 无奇偶校验位 */
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;       /* 无硬件数据流控制 */
	USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;        /* 收发模式 */

	USART_Init( USART1, &USART_InitStructure );                                             /* 初始化串口1 */
	USART_Cmd( USART1, ENABLE );                                                            /* 使能串口1 */
}


static u8	fac_us	= 0;
static u16	fac_ms	= 0;

void delay_init()
{
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK_Div8 ); /*选择外部时钟  HCLK/8 */
	fac_us	= SystemCoreClock / 8000000;                    /* 为系统时钟的1/8 */
	fac_ms	= (u16) fac_us * 1000;
}


void delay_ms( u16 nms )
{
	u32 temp;
	SysTick->LOAD	= (u32) nms * fac_ms;                   /* 时间加载(SysTick->LOAD为24bit) */
	SysTick->VAL	= 0x00;                                 /* 清空计数器 */
	SysTick->CTRL	|= SysTick_CTRL_ENABLE_Msk;             /* 开始倒数 */
	do
	{
		temp = SysTick->CTRL;
	}
	while ( (temp & 0x01) && !(temp & (1 << 16) ) );        /* 等待时间到达 */
	SysTick->CTRL	&= ~SysTick_CTRL_ENABLE_Msk;            /* 关闭计数器 */
	SysTick->VAL	= 0X00;                                 /* 清空计数器 */
}


/* 定时器5通道1输入捕获配置 */
void TIM5_Cap_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	TIM_ICInitTypeDef	TIM5_ICInitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );                  /* 使能TIM5时钟 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );                 /* 使能GPIOA时钟 */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0;                           /* PA0 清除之前设置 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPD;                        /* PA0 输入 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	GPIO_ResetBits( GPIOA, GPIO_Pin_0 );                                    /* PA0 下拉 */

	/* 初始化定时器5 TIM5 */
	TIM_TimeBaseStructure.TIM_Period	= arr;                          /* 设定计数器自动重装值 */
	TIM_TimeBaseStructure.TIM_Prescaler	= psc;                          /* 预分频器 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM向上计数模式 */
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseStructure );                       /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */

	/* 初始化TIM5输入捕获参数 */
	TIM5_ICInitStructure.TIM_Channel	= TIM_Channel_1;                /* CC1S=01      选择输入端 IC1映射到TI1上 */
	TIM5_ICInitStructure.TIM_ICPolarity	= TIM_ICPolarity_Rising;        /*上升沿捕获 */
	TIM5_ICInitStructure.TIM_ICSelection	= TIM_ICSelection_DirectTI;     /*映射到TI1上 */
	TIM5_ICInitStructure.TIM_ICPrescaler	= TIM_ICPSC_DIV1;               /* 配置输入分频,不分频 */
	TIM5_ICInitStructure.TIM_ICFilter	= 0x00;                         /* IC1F=0000 配置输入滤波器 不滤波 */
	TIM_ICInit( TIM5, &TIM5_ICInitStructure );

	/* 中断分组初始化 */
	NVIC_InitStructure.NVIC_IRQChannel			= TIM5_IRQn;    /* TIM3中断 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;            /* 先占优先级2级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;            /* 从优先级0级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;       /* IRQ通道被使能 */
	NVIC_Init( &NVIC_InitStructure );                                       /* 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 */

	TIM_ITConfig( TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE );               /* 允许更新中断 ,允许CC1IE捕获中断 */

	TIM_Cmd( TIM5, ENABLE );                                                /* 使能定时器5 */
}


u8	TIM5CH1_CAPTURE_STA = 0;                                                /* 输入捕获状态 */
u16	TIM5CH1_CAPTURE_VAL;                                                    /* 输入捕获值 */

int main( void )
{
	u32 temp = 0;
	delay_init();                                                           /* 延时函数初始化 */
	uart_init( 115200 );                                                    /* 串口初始化为115200 */

	TIM5_Cap_Init( 0XFFFF, 72 - 1 );                                        /* 以1Mhz的频率计数 */
	while ( 1 )
	{
		delay_ms( 10 );
		if ( TIM5CH1_CAPTURE_STA & 0X80 )               /* 成功捕获到了一次上升沿 */
		{
			temp	= TIM5CH1_CAPTURE_STA & 0X3F;
			temp	*= 65536;                       /* 溢出时间总和 */
			temp	+= TIM5CH1_CAPTURE_VAL;         /* 得到总的高电平时间 */
			printf( "HIGH:%d us\r\n", temp );       /* 打印总的高点平时间 */
			TIM5CH1_CAPTURE_STA = 0;                /* 开启下一次捕获 */
		}
	}
}


/* 定时器5中断服务程序 */
void TIM5_IRQHandler( void )
{
	if ( (TIM5CH1_CAPTURE_STA & 0X80) == 0 )                                        /* 还未成功捕获 */
	{
		if ( TIM_GetITStatus( TIM5, TIM_IT_Update ) != RESET )

		{
			if ( TIM5CH1_CAPTURE_STA & 0X40 )                               /* 已经捕获到高电平了 */
			{
				if ( (TIM5CH1_CAPTURE_STA & 0X3F) == 0X3F )             /* 高电平太长了 */
				{
					TIM5CH1_CAPTURE_STA	|= 0X80;                /* 标记成功捕获了一次 */
					TIM5CH1_CAPTURE_VAL	= 0XFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}
		}
		if ( TIM_GetITStatus( TIM5, TIM_IT_CC1 ) != RESET )                     /* 捕获1发生捕获事件 */
		{
			if ( TIM5CH1_CAPTURE_STA & 0X40 )                               /* 捕获到一个下降沿 */
			{
				TIM5CH1_CAPTURE_STA	|= 0X80;                        /* 标记成功捕获到一次高电平脉宽 */
				TIM5CH1_CAPTURE_VAL	= TIM_GetCapture1( TIM5 );
				TIM_OC1PolarityConfig( TIM5, TIM_ICPolarity_Rising );   /* CC1P=0 设置为上升沿捕获 */
			}else{                                                          /* 还未开始,第一次捕获上升沿 */
				TIM5CH1_CAPTURE_STA	= 0;                            /* 清空 */
				TIM5CH1_CAPTURE_VAL	= 0;
				TIM_SetCounter( TIM5, 0 );
				TIM5CH1_CAPTURE_STA |= 0X40;                            /* 标记捕获到了上升沿 */
				TIM_OC1PolarityConfig( TIM5, TIM_ICPolarity_Falling );  /* CC1P=1 设置为下降沿捕获 */
			}
		}
	}
	TIM_ClearITPendingBit( TIM5, TIM_IT_CC1 | TIM_IT_Update );                      /* 清除中断标志位 */
}



```


