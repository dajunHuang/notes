---
title: STM32 CMSIS与标准固件库文件解析汇总
date: 2022/1/11
---

# startup_stm32f10x_hd.s

&#8194;&#8194;STM32F10x High Density Devices vector table for MDK-ARM toolchain. 
&#8194;&#8194;[STM32启动文件——startup_stm32f10x_hd.s](https://blog.csdn.net/wqx521/article/details/50925553)

# core_cm3.h、core_cm3.c

&#8194;&#8194;CMSIS Cortex-M3 Core Peripheral Access Layer File.
&#8194;&#8194;[core_cm3.c解析]( https://blog.csdn.net/fuyunliushuizjf/article/details/42389171)

# system_stm32f10x.h、system_stm32f10x.c

CMSIS Cortex-M3 Device Peripheral Access Layer System File.

```
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      factors, AHB/APBx prescalers and Flash settings). 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f10x_xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (8 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f10x_xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. If the system clock source selected by user fails to startup, the SystemInit()
  *    function will do nothing and HSI still used as system clock source. User can 
  *    add some code to deal with this issue inside the SetSysClock() function.
  *
  * 4. The default value of HSE crystal is set to 8 MHz (or 25 MHz, depedning on
  *    the product used), refer to "HSE_VALUE" define in "stm32f10x.h" file. 
  *    When HSE is used as system clock source, directly or through PLL, and you
  *    are using different crystal you have to adapt the HSE value to your own
  *    configuration.
```

# stm32f10x_conf.h

&#8194;&#8194;Library configuration file.
&#8194;&#8194;[stm32f10x_conf.h是个什么东东？]( https://blog.csdn.net/tanguohua_666/article/details/88087309)

# stm32f10x.h

&#8194;&#8194;CMSIS Cortex-M3 Device Peripheral Access Layer Header File.
&#8194;&#8194;[stm32f10x.h文件分析理解](https://www.cnblogs.com/caihongsheng/p/3513279.html)
