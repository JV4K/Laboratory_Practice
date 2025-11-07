#include "systick.h"

void SysTick_Init(void) {
    // The SystemCoreClock variable is 168,000,000 after clock configuration
    // 168,000,000 / 1000 = 168,000 ticks for a 1ms interrupt
    if (SysTick_Config(SystemCoreClock / SYSTICK_1MS_TICK)) {
        // Handle error (e.g., reload value out of range)
        while (1);
    }
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
}