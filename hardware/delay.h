#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f0xx.h"

void delay_init(void);
void delay_us(int32_t nus);
void delay_ms(int16_t nms);
#endif
