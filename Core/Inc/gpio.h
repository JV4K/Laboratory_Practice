#include "stm32f4xx.h"
#include "stm32f407xx.h"

#ifndef _INC_GPIO_H_
#define _INC_GPIO_H_

// =============================================================================
// CUSTOM MACRO
// =============================================================================

// === 1. Base peripheral addresses ===
#define MY_RCC_BASE          (0x40023800UL)
#define MY_GPIOB_BASE        (0x40020400UL)

// === RCC registers addresses
#define MY_PERIPH_RCC_AHB1ENR  *(volatile uint32_t *)(MY_RCC_BASE + 0x30UL)
#define MY_RCC_GPIOB_EN_BIT    (1UL << 1)

// === GPIOB registers addresses
#define MY_PERIPH_GPIOB_MODER  *(volatile uint32_t *)(MY_GPIOB_BASE + 0x00UL)
#define MY_PERIPH_GPIOB_OTYPER *(volatile uint32_t *)(MY_GPIOB_BASE + 0x04UL)
#define MY_PERIPH_GPIOB_OSPEEDR *(volatile uint32_t *)(MY_GPIOB_BASE + 0x08UL)
#define MY_PERIPH_GPIOB_PUPDR  *(volatile uint32_t *)(MY_GPIOB_BASE + 0x0CUL)
#define MY_PERIPH_GPIOB_BSRR   *(volatile uint32_t *)(MY_GPIOB_BASE + 0x18UL)

// === Pin 1 Masks ===
#define MY_PIN1_MODER_CLEAR_MASK  (0b11UL << (1 * 2))
#define MY_PIN1_MODER_OUTPUT      (0b01UL << (1 * 2))
#define MY_PIN1_OTYPER_CLEAR_MASK (1UL << 1)
#define MY_PIN1_OSPEEDR_CLEAR_MASK (0b11UL << (1 * 2))
#define MY_PIN1_OSPEEDR_MEDIUM    (0b01UL << (1 * 2))
#define MY_PIN1_PUPDR_CLEAR_MASK  (0b11UL << (1 * 2))
#define MY_PIN1_SET_MASK          (1UL << 1)
#define MY_PIN1_RESET_MASK        (1UL << 17)

void GPIOA_Init_Direct(void);
void GPIOB_Init_Macro(void);
void GPIOC_Init_CMSIS(void);

#endif