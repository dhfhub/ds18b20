#include "exti.h"
//PA9作为输入中断
void EXIT_KEY_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct; 
  	EXTI_InitTypeDef EXTI_InitStruct;
	  NVIC_InitTypeDef NVIC_InitStruct;
	  /* config the extiline(PB0) clock and AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
   
	
	  /* EXTI line gpio config(PA19) */	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
	  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* EXTI line(PA14) mode config */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource14);
		EXTI_InitStruct.EXTI_Line = EXTI_Line9;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct); 
		
		
		 /* Configyre P[A|B|C|D|E]0  NIVC  */
		NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0;//先占优先级0级
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
}



	

