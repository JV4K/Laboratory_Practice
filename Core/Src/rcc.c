#include "rcc.h"

void SystemClock_Config(void) {
    // 1. Enable HSI/HSE and wait for it to be ready
    // Use HSE (8MHz) as the PLL source
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Set Flash Latency for 168MHz (5 wait states)
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

    // 3. Configure AHB/APB Prescalers
    // AHB Prescaler = /1 (HCLK = 168 MHz)
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    // APB1 Prescaler = /4 (PCLK1 = 42 MHz - Max 42 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
    // APB2 Prescaler = /2 (PCLK2 = 84 MHz - Max 84 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    // 4. Configure the Main PLL
    // PLL source is HSE, PLLM=8, PLLN=336, PLLP=2 (0b00)
    // PLLQ = 7 (required for USB/SDIO/RNG, 48MHz for USB is 336/7)
    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos) | 
                   (336 << RCC_PLLCFGR_PLLN_Pos) | 
                   (0 << RCC_PLLCFGR_PLLP_Pos) | // PLLP = /2
                   (7 << RCC_PLLCFGR_PLLQ_Pos) | 
                   RCC_PLLCFGR_PLLSRC_HSE;

    // 5. Enable the PLL and wait for it to be ready
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 6. Switch the System Clock source to PLL
    // SW[1:0] bits 10: PLL selected as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Update the SystemCoreClock variable for accurate timekeeping functions
    SystemCoreClockUpdate();
}
