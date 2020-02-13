#ifndef __INTERNAL_H
#define __INTERNAL_H

#include "stm32f0xx.h"


void internal_flash_unlock(void);
void internal_flash_erase(void);
void internal_flash_clearflag(void);
void internal_flash_lock(void);
void internal_flash_program(void);
void internal_flash_check(void);
void internal_flash_write(u8 flash_write_flag);
uint32_t internal_flash_read(void);
#endif
