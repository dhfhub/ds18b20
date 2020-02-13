#ifndef __LED_H
#define __LED_H

#include "stm32f0xx.h"

//#define LED_OFF                       GPIOA->BSRR = 0x40
//#define LED_ON                        GPIOA->BRR = 0x40 
//#define LED_TURN                      GPIOA->ODR ^= 0x40





void gpio_init(void);


#endif
