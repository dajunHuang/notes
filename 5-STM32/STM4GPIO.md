---
title: STM32学习笔记（四）GPIO
date: 2022/1/7
---
> 这几天先把之前学到一半的东西搬到站上，记录的同时也算复习了XD。

# GPIO初始化
&#8195;&#8195;为了使用GPIO，首先要使用`void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)`函数使能GPIO时钟，比如使能GPIOA时钟方法是：
```
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
```
&#8195;&#8195;在固件库开发中，操作寄存器`CRH`和`CRL`来配置IO口的模式和速度是通过GPIO初始化函数`void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)`完成，这个函数有两个参数，第一个参数是用来指定GPIO，取值范围为GPIOA~GPIOG。第二个参数为初始化参数结构体指针，结构体类型为`GPIO_InitTypeDef`：
```
typedef struct
{ 
    uint16_t GPIO_Pin; 
    GPIOSpeed_TypeDef GPIO_Speed; 
    GPIOMode_TypeDef GPIO_Mode; 
}GPIO_InitTypeDef;
```
&#8195;&#8195;通过初始化结构体初始化GPIO的常用格式是：
```
GPIO_InitTypeDef GPIO_InitStructure; 
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //LED0-->PB.5 端口配置 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度 50MHz 
GPIO_Init(GPIOB, &GPIO_InitStructure);//根据设定参数配置 GPIO
```
## GPIO_Mode
&#8195;&#8195;`GPIO_Mode`是用来设置对应IO端口的输出输入模式，8个模式是通过一个枚举类型定义的：
```
typedef enum { 
    GPIO_Mode_AIN = 0x0, //模拟输入 
    GPIO_Mode_IN_FLOATING = 0x04, //浮空输入 
    GPIO_Mode_IPD = 0x28,  //下拉输入 
    GPIO_Mode_IPU = 0x48, //上拉输入 
    GPIO_Mode_Out_OD = 0x14, //开漏输出 
    GPIO_Mode_Out_PP = 0x10, //通用推挽输出 
    GPIO_Mode_AF_OD = 0x1C, //复用开漏输出 
    GPIO_Mode_AF_PP = 0x18 //复用推挽 
}GPIOMode_TypeDef;
```
1. 上拉输入：上拉就是把电位拉高，比如拉到Vcc。上拉就是将不确定的信号通过一个电阻嵌位在高电平电阻同时起限流作用弱强只是上拉电阻的阻值不同，没有什么严格区分。
2. 下拉输入：就是把电压拉低，拉到GND。与上拉原理相似。
3. 浮空输入：浮空）就是逻辑器件的输入引脚即不接高电平，也不接低电平。由于逻辑器件的内部结构，当它输入引脚悬空时，相当于该引脚接了高电平。一般实际运用时，引脚不建议悬空，易受干扰。通俗讲就是让管脚什么都不接，浮空着。
4. 模拟输入：模拟输入是指传统方式的输入。数字输入是输入PCM数字信号，即0，1的二进制数字信号，通过数模转换，转换成模拟信号，经前级放大进入功率放大器，功率放大器还是模拟的。
5. 推挽输出:可以输出高，低电平，连接数字器件。推挽结构一般是指两个三极管分别受两互补信号的控制，总是在一个三极管导通的时候另一个截止。高低电平由IC的电源低定。
6. 开漏输出:输出端相当于三极管的集电极。要得到高电平状态需要上拉电阻才行。适合于做电流型的驱动，其吸收电流的能力强，一般20mA以内。
7. 复用输出：可以理解为GPIO口被用作第二功能时的配置情况。端口必须配置成复用功能输出模式（推挽或开漏）。
## GPIO_Speed
`GPIO_Speed`是IO口速度，有三个可选值：
```
typedef enum { 
    GPIO_Speed_10MHz = 1, 
    GPIO_Speed_2MHz, 
    GPIO_Speed_50MHz 
}GPIOSpeed_TypeDef;
```
# GPIO库函数
## 读取数据
&#8195;&#8195;在固件库中操作`IDR`寄存器读取IO端口数据是通过`uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`函数实现的，比如我要读GPIOA.5的电平状态，那么方法是：
```
GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
```
&#8195;&#8195;返回值是1(Bit_SET)或者0(Bit_RESET)。
## 输出数据
&#8195;&#8195;在STM32固件库中，通过`BSRR`和`BRR`寄存器设置GPIO端口输出是通过函数`void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`和函数`void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`来完成的。在多数情况下，我们都是采用这两个函数来设置GPIO端口的输入和输出状态。比如我们要设置GPIOB.5输出1，那么方法为：
```
GPIO_SetBits(GPIOB, GPIO_Pin_5);
```

&#8195;&#8195;综上，GPIO操作步骤为：
1. 使能IO口时钟。 
2. 初始化IO参数。 
3. 操作IO。


**参考：正点原子STM32F1开发指南-库函数版本_V3.3、STM32中文参考手册_V10**

---
# 实例
```
/*
 * 本实验,通过开发板上载有的4个按钮（KEY_UP、KEY0、KEY1和KEY2）
 * 来控制板上的2个LED（DS0和DS1）和蜂鸣器，其中WK_UP控制蜂鸣器，按一次叫，再按一次停；
 * KEY2控制DS0，按一次亮，再按一次灭；KEY1控制DS1，效果同KEY2；
 * KEY0则同时控制DS0和DS1，按一次，他们的状态就翻转一次。
 */

#include "stm32f10x.h"
#define BITBAND( addr, bitnum )		( (addr & 0xF0000000) + 0x2000000 + ( (addr & 0xFFFFF) << 5) + (bitnum << 2) )
#define MEM_ADDR( addr )		*( (volatile unsigned long *) (addr) )
#define BIT_ADDR( addr, bitnum )	MEM_ADDR( BITBAND( addr, bitnum ) )
#define GPIOB_ODR_Addr	(GPIOB_BASE + 12)                               /* 0x40010C0C */
#define GPIOE_ODR_Addr	(GPIOE_BASE + 12)                               /* 0x4001180C */
#define PBout( n )	BIT_ADDR( GPIOB_ODR_Addr, n )                   /* 输出 */
#define PBin( n )	BIT_ADDR( GPIOB_IDR_Addr, n )                   /* 输入 */
#define PEout( n )	BIT_ADDR( GPIOE_ODR_Addr, n )                   /* 输出 */
#define PEin( n )	BIT_ADDR( GPIOE_IDR_Addr, n )                   /* 输入 */
#define BEEP		PBout( 8 )                                      /* BEEP,蜂鸣器接口 */
#define LED0		PBout( 5 )                                      /* PB5 */
#define LED1		PEout( 5 )                                      /* PE5 */
#define KEY0		GPIO_ReadInputDataBit( GPIOE, GPIO_Pin_4 )      /* 读取按键0 */
#define KEY1		GPIO_ReadInputDataBit( GPIOE, GPIO_Pin_3 )      /* 读取按键1 */
#define KEY2		GPIO_ReadInputDataBit( GPIOE, GPIO_Pin_2 )      /* 读取按键2 */
#define WK_UP		GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 )      /* 读取按键3(WK_UP) */
#define KEY0_PRES	1                                               /* KEY0按下 */
#define KEY1_PRES	2                                               /* KEY1按下 */
#define KEY2_PRES	3                                               /* KEY2按下 */
#define WKUP_PRES	4                                               /* KEY_UP按下(即WK_UP/KEY_UP) */

static u8	fac_us	= 0;
static u16	fac_ms	= 0;

void delay_init()
{
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK_Div8 );         /*选择外部时钟  HCLK/8 */
	fac_us	= SystemCoreClock / 8000000;                            /* 为系统时钟的1/8 */
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


void BEEP_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );                         /* 使能GPIOB端口时钟 */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_8;                                   /* BEEP-->PB.8 端口配置 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;                             /* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;                             /* 速度为50MHz */
	GPIO_Init( GPIOB, &GPIO_InitStructure );                                        /* 根据参数初始化GPIOB.8 */

	GPIO_ResetBits( GPIOB, GPIO_Pin_8 );                                            /* 输出0，关闭蜂鸣器输出 */
}


void KEY_Init( void )                                                                   /* IO初始化 */
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE );  /* 使能PORTA,PORTE时钟 */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;         /* KEY0-KEY2 */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;                                /* 设置成上拉输入 */
	GPIO_Init( GPIOE, &GPIO_InitStructure );                                        /* 初始化GPIOE2,3,4 */

	/* 初始化 WK_UP-->GPIOA.0	  下拉输入 */
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPD;                                /* PA0设置成输入，默认下拉 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                        /* 初始化GPIOA.0 */
}


/*
 * 键处理函数
 * 返回按键值
 * mode:0,不支持连续按(长按识别为一次触发);1,支持连续按(长按识别为多次触发);
 * 0，没有任何按键按下
 * 1，KEY0按下
 * 2，KEY1按下
 * 3，KEY2按下
 * 4，KEY3按下 WK_UP
 * 注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
 */
u8 KEY_Scan( u8 mode )
{
	static u8 key_up = 1;                                                           /*按键按松开标志 */
	if ( mode )
		key_up = 1;                                                             /* 支持连按 */
	if ( key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1) )
	{
		delay_ms( 10 );                                                         /* 去抖动 */
		key_up = 0;
		if ( KEY0 == 0 )
			return(KEY0_PRES);
		else if ( KEY1 == 0 )
			return(KEY1_PRES);
		else if ( KEY2 == 0 )
			return(KEY2_PRES);
		else if ( WK_UP == 1 )
			return(WKUP_PRES);
	}else if ( KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0 )
		key_up = 1;
	return(0);                              /* 无按键按下 */
}


int main( void )
{
	vu8 key = 0;
	delay_init();                           /* 延时函数初始化 */
	LED_Init();                             /* LED端口初始化 */
	KEY_Init();                             /* 初始化与按键连接的硬件接口 */
	BEEP_Init();                            /* 初始化蜂鸣器端口 */
	GPIO_ResetBits( GPIOB, GPIO_Pin_5 );    /* 先点亮红灯 */
	LED0 = 0;                               /* 先点亮红灯 */
	while ( 1 )
	{
		key = KEY_Scan( 0 );            /* 得到键值 */
		if ( key )
		{
			switch ( key )
			{
			case WKUP_PRES:         /* 控制蜂鸣器 */
				BEEP = !BEEP;
				break;
			case KEY2_PRES:         /* 控制LED0翻转 */
				LED0 = !LED0;
				break;
			case KEY1_PRES:         /* 控制LED1翻转 */
				LED1 = !LED1;
				break;
			case KEY0_PRES:         /*同时控制LED0,LED1翻转 */
				LED0	= !LED0;
				LED1	= !LED1;
				break;
			}
		}else delay_ms( 10 );
	}
}



```
