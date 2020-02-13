#ifndef __WDOG_H
#define __WDOG_H

#include "stm32f0xx.h"
void iwdg_init(void);
void iwdg_feed(void);
void wwdg_init(void);

#endif
