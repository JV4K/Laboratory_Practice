#include "stm32f4xx.h"
#include "stm32f407xx.h"

#ifndef _INC_SYSTICK_H_
#define _INC_SYSTICK_H_

#define SYSTICK_1MS_TICK 1000 // Ticks per second
void SysTick_Init(void);

#endif