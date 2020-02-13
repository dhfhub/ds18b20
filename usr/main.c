#include "stm32f0xx.h"
#include "led.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "delay.h"
#include "ds18b20.h"
#include "wdog.h"
#include "exti.h"
#include <stdio.h>



#define LED_OFF                       GPIOA->BSRR = 0x40
#define LED_ON                        GPIOA->BRR = 0x40 
#define LED_TURN                      GPIOA->ODR ^= 0x40

#define LIGHT1_ON                       GPIOA->BSRR = 0x10
#define LIGHT1_OFF                        GPIOA->BRR = 0x10 
#define LIGHT1_TURN                      GPIOA->ODR ^= 0x10

#define LIGHT_ON                       GPIOA->BSRR = 0x80//LIGHT_ON
#define LIGHT_OFF                       GPIOA->BRR = 0x80//LIGHT_OFF
#define LIGHT_TURN											GPIOA->ODR ^= 0x80

#define BEEP_OFF  											 GPIO_SetBits(GPIOA,GPIO_Pin_5);	//高电平
#define BEEP_ON                         GPIO_ResetBits(GPIOA,GPIO_Pin_5);//低电平
#define BEEP_TURN 											GPIOA->ODR ^= 0x20
#define disable_irq() __disable_irq() 
#define enable_irq() __enable_irq() 


//温度与温度传感器相关
u8 T_cunzai=0;
u8 T_set;//设定温度值
extern u8 ds18b20_id[2][8];
short temperature;
//报警温度设置相关
u8 first_get_T_flag;//用于程序开始采集温度设置值，采集完成后清零，除非系统复位重新采集

//多次采集温度
u8 get_temp_times_over_zero;
u8 get_temp_times_over_fifteen;

int main(void)
{
	/*系统时钟初始化为48M
	各个时钟如下:
	LSI_RC=40K	HSI_RC=8M HSE=8M HSI14_RC=14M
	PLL=SYSCLK=HCLK=AHB=FCLK=APB1=48M
	*/
	SystemInit();
	/*将systick作为系统延时精度定时器*/
	delay_init();
	usart_init();
	gpio_init();
	LED_OFF;
	delay_ms(500);
	LIGHT_OFF;
	LIGHT1_OFF;

	while(DS18B20_Init())		
	{
 		delay_ms(200);
	}	
	T_cunzai = 1;
	//printf("DS18B20 OK\n");
	/*读取温度传感器ID*/
	DS18B20_ReadID(*ds18b20_id);
	
	//温度设置采集
	first_get_T_flag=1;//采集完成后清零
	
	/*定时器中断初始化*/
	timer14_init();
	
	
	
	
	while(1)          
	{	
		temperature=DS18B20S_Get_Temp(1);
		/*将温度传感器值打印到串口显示*/
		printf("\n");
		printf("temperature=%d",temperature/10);
		if(temperature>=150)
		{
			LED_ON;
			get_temp_times_over_fifteen++;
			if(get_temp_times_over_fifteen >=10)
			{
				LIGHT1_OFF;//关闭加热模块
				get_temp_times_over_fifteen = 0;
			}
			
		}
		else
		{
			LED_OFF;
			get_temp_times_over_fifteen = 0;
		}
		
		if(temperature>=0)
		{
			get_temp_times_over_zero++;
			if(get_temp_times_over_zero>=10)
			{
				LIGHT_ON;//电源开
				get_temp_times_over_zero = 0;
			}
			
		}
		else if(temperature<0)
		{
			LIGHT1_ON;//启动加热模块
			LIGHT_OFF;//电源关
			get_temp_times_over_zero = 0;
		}
		
	}
	
	

}

/*1s中断一次*/
void TIM14_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		disable_irq();
		
		enable_irq();	
	}
}


/*外部中断输入*/
void EXTI4_15_IRQHandler(void)
{  
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)  //进入中断
	{  
		
		
	}  

	EXTI_ClearFlag(EXTI_Line9);  
} 



