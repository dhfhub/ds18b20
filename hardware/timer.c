#include "timer.h"
#include "led.h"
#include "wdog.h"

//根据散热风扇的频率 ，设置为25Khz
void pwm_pb1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       TIM_OCInitStructure;
	
	//配置PB1为TIM3_CH4的复用PWM输出
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);
	
	//配置TIM3
	/*
	TIM3由APB1总线使能，APB1时钟为48M，在SYSCLK和HCLK被配置为48M情况下
	25K=48M=1920
	
	设置
	TIM_Prescaler=0
	TIM_ClockDivision=0
	TIM_Period=1920
	
	设置PWM1模式，计数完成前输出高电平
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟
	TIM_TimeBaseStructure.TIM_Period        = 1920;// 设置自动重装周期值
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;//设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //初始化定时器3

	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;// PWM1模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;//计数完成前输出高，后输出低
  TIM_OCInitStructure.TIM_Pulse = PWM_OFF;

	TIM_OC4Init(TIM3, &TIM_OCInitStructure);//通道4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能预装载寄存器
	TIM_Cmd(TIM3, ENABLE);
}

//改变pwm占空比
void chang_pb1_pwm_duty_cycle(uint16_t CCRx_Val)
{
	TIM3->CCR4=CCRx_Val;
	TIM_Cmd(TIM3, ENABLE);
}

void timer14_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); 
	TIM_TimeBaseStructure.TIM_Period        = 59999;// 设置自动重装周期值
	TIM_TimeBaseStructure.TIM_Prescaler     = 799;//设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);    //初始化定时器3
	
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM14, //TIM14
		TIM_IT_Update ,
		ENABLE  //使能
		);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;  //先占优先级1级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM14, ENABLE);  //使能TIMx外设
}



#if 0
uint16_t PrescalerValue = 0;
__IO uint16_t CCR1_Val = 10000;
__IO uint16_t CCR2_Val = 20000;
__IO uint16_t CCR3_Val = 30000;
__IO uint16_t CCR4_Val = 40000;
__IO uint16_t temp1;
__IO uint16_t temp2;
__IO uint16_t temp3;
__IO uint16_t temp4;
uint16_t capture = 0;
void timer_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟
	/* -----------------------------------------------------------------------
	TIM3 配置: 输出比较时序模式:

	本试验中 TIM3输入时钟(TIM3CLK) 被设为APB1 时钟 (PCLK1),  
		=> TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
				
	当 TIM3 计数时钟 设为6 MHz, 预分频器可以按照下面公式计算：
		 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
		 Prescaler = (PCLK1 /6 MHz) - 1
	
	本实验TIM3 counter clock=48M=48000000
	CC1 翻转率 = TIM3 counter clock / CCR1_Val = 4800 Hz
	CC2 翻转率= TIM3 counter clock / CC2_Val = 2400 Hz
	CC3 翻转率 = TIM3 counter clock / CCR3_Val = 1600 Hz
	CC4 翻转率= TIM3 counter clock / CCR4_Val = 1200 Hz
	----------------------------------------------------------------------- */
  /* Time 定时器基础设置 */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* 预分频器配置 */
  TIM_PrescalerConfig(TIM3, 0, TIM_PSCReloadMode_Immediate);

  /* 输出比较时序模式配置设置 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	/* 输出比较时序模式配置: 频道1*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* 输出比较时序模式配置: 频道2*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* 输出比较时序模式配置: 频道3*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* 输出比较时序模式配置: 频道4*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
   
  /* TIM 中断使能 */
  TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_CC2| TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

  /* TIM3 使能 */
  TIM_Cmd(TIM3, ENABLE);
}

#endif


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		}
	
#if 0	
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
     temp1++;
    /* LED3 toggling with frequency = 219.7 Hz */
		LED_TURN;
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare1(TIM3, capture + CCR1_Val);
  }
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
     temp2++;
    /* LED3 toggling with frequency = 219.7 Hz */
		
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare2(TIM3, capture + CCR2_Val);
  }
  if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
     temp3++;
    /* LED3 toggling with frequency = 219.7 Hz */
		
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare3(TIM3, capture + CCR3_Val);
  }
  if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
     temp4++;
    /* LED4 toggling with frequency = 439.4 Hz */

    capture = TIM_GetCapture4(TIM3);
    TIM_SetCompare4(TIM3, capture + CCR4_Val);
  }
#endif
}






