#ifndef _SPI_MAX31865_H
#define _SPI_MAX31865_H


#include "stm32f0xx.h"
#define SPI_MAX31865_SPI SPI1

//CS-PB1
#define SPI_MAX31865_CS_PIN        GPIO_Pin_1
#define SPI_MAX31865_CS_GPIO_PORT       GPIOB


//DRDY-PA4
#define DRDY_PIN        GPIO_Pin_4
#define DRDY_PORT       GPIOA

//SCK-PA5
#define SPI_MAX31865_SPI_SCK_PIN        GPIO_Pin_5
#define SPI_MAX31865_SPI_SCK_GPIO_PORT       GPIOA
#define SPI_MAX31865_SPI_SCK_SOURCE     GPIO_PinSource5
#define SCK_AF         GPIO_AF_5

//MISO-PA6
#define SPI_MAX31865_SPI_MISO_PIN       GPIO_Pin_6
#define SPI_MAX31865_SPI_MISO_GPIO_PORT      GPIOA
#define SPI_MAX31865_SPI_MISO_SOURCE    GPIO_PinSource6
#define MISO_AF        GPIO_AF_5

//MOSI-PA7
#define SPI_MAX31865_SPI_MOSI_PIN       GPIO_Pin_7
#define SPI_MAX31865_SPI_MOSI_GPIO_PORT      GPIOA
#define SPI_MAX31865_SPI_MOSI_SOURCE    GPIO_PinSource7
#define MOSI_AF        GPIO_AF_5


#define SPI_MAX31865_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define SPI_MAX31865_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_1)



uint8_t sEE_SendByte(uint8_t value);
void SPI_MAX31865_Write(u8 address, u8 value)      ;
uint8_t SPI_MAX31865_Read(u8 address)   ;

#endif
