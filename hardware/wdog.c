#include "wdog.h"

//独立看门狗，用于精度不高的场合
//iwdg由内部低速RC振荡器提供时钟40K
void iwdg_init(void)
{
	//IWDG寄存器写使能
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//IWDG时钟分频40K/256=156HZ(6.4ms)
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	//喂狗时间，注意不能大于0xfff
	IWDG_SetReload(160);//(160/156)s需喂狗一次，否则复位,所以设置tim14每秒进行喂狗
	//喂狗
	IWDG_ReloadCounter();
	//使能看门狗
	IWDG_Enable();
}
//喂独立看门狗
void iwdg_feed()
{
	IWDG_ReloadCounter();		
}


#if 0
//窗口看门狗，用于精度高场合(喂狗早了，系统复位，喂狗晚了，系统复位，还可设置中断)
void wwdg_init(void)
{
//	//WWDG时钟使能
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
//	//WWDG时钟分频
//	WWDG_SetPrescaler(WWDG_Prescaler_8);
//	//窗口值设置,达到0x42之前喂狗就算喂早了
//	WWDG_SetWindowValue(0x42);
//	//初始值0x7F
//	WWDG_Enable(0x7F);
//	//清除中断标志
//	WWDG_ClearFlag();
//	//使能中断
//	WWDG_EnableIT();
}

#endif







