#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f0xx.h"

#define PWM_OFF 10
#define PWM_LOW_SPEED 100
#define PWM_MID_SPEED 900
#define PWM_MAX_SPEED 1900

void pwm_pb1_init(void);
void timer_init(void);
void pwm_init(uint16_t period, uint16_t prescaler);
void chang_pb1_pwm_duty_cycle(uint16_t CCRx_Val);
void timer14_init(void);
#endif
