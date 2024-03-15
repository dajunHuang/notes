---
title: STM32学习笔记（八）PWM
date: 2022/1/9
---
&#8195;&#8195;脉冲宽度调制(PWM)，是英文“Pulse Width Modulation”的缩写，简称脉宽调制。STM32的定时器除了`TIM6`和`TIM7`。其他的定时器都可以用来产生PWM输出。其中高级定时器`TIM1`和`TIM8`可以同时产生多达7路的PWM输出。而通用定时器也能同时产生多达4路的PWM输出，这样，STM32最多可以同时产生30路PWM输出，这里我们仅利用`TIM3`的`CH2`产生一路PWM输出。PWM相关的函数设置在库函数文件`stm32f10x_tim.h`和`stm32f10x_tim.c`文件中。
# 重映射和计时器配置
&#8195;&#8195;这部分前面已经介绍过，这里不再展开。

# PWM配置
&#8195;&#8195;在库函数中，PWM通道设置是通过函数`TIM_OC1Init()`~ `TIM_OC4Init()`来设置的，这里我们使用的是通道2，所以使用的函数是`void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)`，结构体`TIM_OCInitTypeDef`的定义如下：
```
typedef struct
{
  uint16_t TIM_OCMode;      
  uint16_t TIM_OutputState; 
  uint16_t TIM_OutputNState; 
  uint16_t TIM_Pulse; 
  uint16_t TIM_OCPolarity; 
  uint16_t TIM_OCNPolarity; 
  uint16_t TIM_OCIdleState; 
  uint16_t TIM_OCNIdleState; 
} TIM_OCInitTypeDef;
```
&#8195;&#8195;第一个参数`TIM_OCMode`设置模式是PWM还是输出比较，取值如下：
```
#define TIM_OCMode_Timing                  ((uint16_t)0x0000)
#define TIM_OCMode_Active                  ((uint16_t)0x0010)
#define TIM_OCMode_Inactive                ((uint16_t)0x0020)
#define TIM_OCMode_Toggle                  ((uint16_t)0x0030)
#define TIM_OCMode_PWM1                    ((uint16_t)0x0060)
#define TIM_OCMode_PWM2                    ((uint16_t)0x0070)
```
&#8195;&#8195;PWM模式1：在向上计数时，一旦`TIMx_CNT` < `TIMx_CCR1`时通道为有效电平，否则为无效电平；在向下计数时，一旦`TIMx_CNT` > `TIMx_CCR1`时通道为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)。
&#8195;&#8195;PWM模式2：在向上计数时，一旦`TIMx_CNT` < `TIMx_CCR1`时通道为无效电平，否则为有效电平；在向下计数时，一旦`TIMx_CNT` > `TIMx_CCR1`时通道为有效电平，否则为无效电平。
&#8195;&#8195;第二个参数`TIM_OutputState`用来设置比较输出使能，也就是使能PWM输出到端口，取值如下：
```
#define TIM_OutputState_Disable            ((uint16_t)0x0000)
#define TIM_OutputState_Enable             ((uint16_t)0x0001)
```
&#8195;&#8195;第四个参数`TIM_Pulse`用来设置占空比，和`TIM_TimeBaseStructure.TIM_Period`的预装载值对应。修改`TIMx_CCR2`占空比的函数是`void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2)`，例如修改`TIM3_CCR2`占空比的方法是：
```
TIM_SetCompare2(TIM3,8000);
```
&#8195;&#8195;第五个参数`TIM_OCPolarity`用来设置极性是高还是低，即高电平有效或者低电平有效。
&#8195;&#8195;其他的参数`TIM_OutputNState`，`TIM_OCNPolarity`，`TIM_OCIdleState`和`TIM_OCNIdleState`是高级定时器`TIM1`和`TIM8`才用到的。PWM配置格式为：
```
TIM_OCInitTypeDef TIM_OCInitStructure; 
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择 PWM 模式 2 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性高 
TIM_OC2Init(TIM3, &TIM_OCInitStructure); //初始化 TIM3 OC2
```
&#8195;&#8195;在完成以上设置了之后，我们需要使能`TIM3`：
```
TIM_Cmd(TIM3, ENABLE); //使能 TIM3
```

&#8195;&#8195;综上，输出PWM的一般步骤：
1. GPIO重映射配置。
2. 初始化计时器。
3. 配置使能计时器的PWM输出。
4. 使能计时器。


**参考：正点原子STM32F1开发指南-库函数版本_V3.3、STM32中文参考手册_V10**

---
示例：
```
/*
 * 本实验,DS0由暗到亮,再由亮到暗,再由暗到亮,依次循环.
 */

#include "stm32f10x.h"
#define BITBAND( addr, bitnum )		( (addr & 0xF0000000) + 0x2000000 + ( (addr & 0xFFFFF) << 5) + (bitnum << 2) )
#define MEM_ADDR( addr )		*( (volatile unsigned long *) (addr) )
#define BIT_ADDR( addr, bitnum )	MEM_ADDR( BITBAND( addr, bitnum ) )
#define GPIOB_ODR_Addr	(GPIOB_BASE + 12)                       /* 0x40010C0C */
#define GPIOE_ODR_Addr	(GPIOE_BASE + 12)                       /* 0x4001180C */
#define PBout( n )	BIT_ADDR( GPIOB_ODR_Addr, n )           /* 输出 */
#define PBin( n )	BIT_ADDR( GPIOB_IDR_Addr, n )           /* 输入 */
#define PEout( n )	BIT_ADDR( GPIOE_ODR_Addr, n )           /* 输出 */
#define PEin( n )	BIT_ADDR( GPIOE_IDR_Addr, n )           /* 输入 */
#define LED0	PBout( 5 )                                      /* PB5 */
#define LED1	PEout( 5 )                                      /* PE5 */

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


void LED_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE );  /* 使能PB,PE端口时钟 */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_5;                                   /* LED0-->PB.5 端口配置 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;                             /* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;                             /* IO口速度为50MHz */
	GPIO_Init( GPIOB, &GPIO_InitStructure );                                        /* 根据设定参数初始化GPIOB.5 */
	GPIO_SetBits( GPIOB, GPIO_Pin_5 );                                              /* PB.5 输出高 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                                       /* LED1-->PE.5 端口配置, 推挽输出 */
	GPIO_Init( GPIOE, &GPIO_InitStructure );                                        /* 推挽输出 ，IO口速度为50MHz */
	GPIO_SetBits( GPIOE, GPIO_Pin_5 );                                              /* PE.5 输出高 */
}


/*
 * TIM3 PWM部分初始化
 * PWM输出初始化2
 * arr：自动重装值
 * psc：时钟预分频数
 */
void TIM3_PWM_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef	TIM_OCInitStructure;


	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );                          /* 使能定时器3时钟 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );   /* 使能GPIO外设和AFIO复用功能模块时钟 */

	GPIO_PinRemapConfig( GPIO_PartialRemap_TIM3, ENABLE );                          /* Timer3部分重映射  TIM3_CH2->PB5 */

	/* 设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5 */
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_5;                                   /* TIM_CH2 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                              /*复用推挽输出 */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );                                        /* 初始化GPIO */

	/* 初始化TIM3 */
	TIM_TimeBaseStructure.TIM_Period	= arr;                                  /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
	TIM_TimeBaseStructure.TIM_Prescaler	= psc;                                  /* 设置用来作为TIMx时钟频率除数的预分频值 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                        /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;                   /* TIM向上计数模式 */
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );                               /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */

	/* 初始化TIM3 Channel2 PWM模式 */
	TIM_OCInitStructure.TIM_OCMode	= TIM_OCMode_PWM2;                      /*选择定时器模式:TIM脉冲宽度调制模式2 */
	TIM_OCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;               /* 比较输出使能 */
	TIM_OCInitStructure.TIM_OCPolarity	= TIM_OCPolarity_High;                  /* 输出极性:TIM输出比较极性高 */
	//TIM_OCInitStructure.TIM_Pulse = ****;
	TIM_OC2Init( TIM3, &TIM_OCInitStructure );                                      /* 根据T指定的参数初始化外设TIM3 OC2 */

	TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable );                             /* 使能TIM3在CCR2上的预装载寄存器 */

	TIM_Cmd( TIM3, ENABLE );                                                        /* 使能TIM3 */
}


int main( void )
{
	u16	led0pwmval	= 0;
	u8	dir		= 1;
	delay_init();                                           /* 延时函数初始化 */
	LED_Init();                                             /* LED端口初始化 */
	TIM3_PWM_Init( 899 , 0 );                                /*不分频。PWM频率=72000000/900=80Khz */
	while ( 1 )
	{
		delay_ms( 10 );
		if ( dir )
			led0pwmval++;
		else led0pwmval--;
		if ( led0pwmval > 300 )
			dir = 0;
		if ( led0pwmval == 0 )
			dir = 1;
		TIM_SetCompare2( TIM3, led0pwmval );
	}
}



```
