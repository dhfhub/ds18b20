#ifndef __MAX31865_H
#define __MAX31865_H

#include "stm32f0xx.h"



#define SPI_ReceiveData() SPI_I2S_ReceiveData16(SPI1)
#define SPI_SendData(n)	SPI_I2S_SendData16(SPI1,n);



#define SPI_FAIL       1
#define SPI_OK         0
#define Long_TIME_OUT  (uint32_t)0xFFFFFF;


//CS-PB1
#define CS_PIN        GPIO_Pin_1
#define CS_PORT       GPIOB


//DRDY-PA4
#define DRDY_PIN        GPIO_Pin_4
#define DRDY_PORT       GPIOA

//SCK-PA5
#define SCK_PIN        GPIO_Pin_5
#define SCK_PORT       GPIOA
#define SCK_SOURCE     GPIO_PinSource5
#define SCK_AF         GPIO_AF_0

//MISO-PA6
#define MISO_PIN       GPIO_Pin_6
#define MISO_PORT      GPIOA
#define MISO_SOURCE    GPIO_PinSource6
#define MISO_AF        GPIO_AF_0

//MOSI-PA7
#define MOSI_PIN       GPIO_Pin_7
#define MOSI_PORT      GPIOA
#define MOSI_SOURCE    GPIO_PinSource7
#define MOSI_AF        GPIO_AF_0

#define SPI_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define SPI_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_1)



void SPI_MAX31865_Init(void);
u8 SPI_ReadWriteByte(u8 TxData);
u8 SPI_Operation(uint8_t Register_Address,uint8_t *buff,uint8_t nBytes);
void Inital_MAX31865(void);
void Fault_Detect(void);
unsigned char DRDY_Status(void);


#endif


