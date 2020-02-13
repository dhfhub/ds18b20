#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f0xx.h"

#define num_of_sensor 2	//温度传感器个数



typedef enum {FAILED = 0,PASSED = !FAILED} TestStatus;

// ROM COMMANDS
#define ROM_Search_Cmd 0xF0
#define ROM_Read_Cmd   0x33
#define ROM_Match_Cmd  0x55
#define ROM_Skip_Cmd   0xCC
#define ROM_AlarmSearch_Cmd 0xEC

// DS18B20 FUNCTION COMMANDS
#define Convert_T         0x44
#define Write_Scratchpad  0x4E
#define Read_Scratchpad   0xBE
#define Copy_Scratchpad   0x48
#define Recall_EEPROM     0xB8
#define Read_PowerSupply  0xB4

//IO方向设置
#define DS18B20_IO_OUT() {GPIOA->MODER&=~0x11;GPIOA->MODER|=0x01;GPIOA->OTYPER&=~0x01;GPIOA->OTYPER|=0x00;}//设置PA0输出 推挽
#define DS18B20_IO_IN()  {GPIOA->MODER&=~0x11;GPIOA->MODER|=0x00;}//设置PA0输入

#define DS18B20_DQ_OUT_H GPIO_SetBits(GPIOA,GPIO_Pin_0)	//设置PA0=1
#define DS18B20_DQ_OUT_L GPIO_ResetBits(GPIOA,GPIO_Pin_0)	//设置PA0=0

#define DS18B20_DQ_IN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//获取PA0的数据
   	
u8 DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
short DS18B20S_Get_Temp(u8 num);//多个18b20获取温度程序
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20   
void DS18B20_ReadID(unsigned char *p);//读取ID
unsigned char Read_ROM(void);//读取18b20内部ROM
#endif















