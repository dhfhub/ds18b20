#include "led.h"

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* 使能GPIOB时钟 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* 配置LED相应引脚PA4,beep-PA5,LIGHT--PA7,*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//LED_ON
//	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//LED_ON
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);//LIGHT_ON
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//LIGHT1_ON
	GPIO_SetBits(GPIOA,GPIO_Pin_5);//BEEP_OFF
}

