#include "main.h" // Предполагается, что здесь есть все: main.h, button_debouncer.h, CMSIS-style definitions, и SystemClock_Config, SysTick_Init и GPIO_Init функции.

// Globals
uint64_t millis = 0;
Button_t btn1_sequence;
Button_t btn2_count;

// Led logic states
uint8_t current_led_index = 0;
uint8_t active_led_count = 1;

// LED CMSIS MACRO
#define LED1_PIN_BIT 1 // PA1
#define LED2_PIN_BIT 1 // PB1
#define LED3_PIN_BIT 2 // PC2
#define LED_ON(PORT, PIN_BIT)  SET_BIT(PORT->BSRR, (1U << (PIN_BIT)))
#define LED_OFF(PORT, PIN_BIT) SET_BIT(PORT->BSRR, (1U << ((PIN_BIT) + 16U)))

void update_leds() {
    // 1. Выключаем все светодиоды
    LED_OFF(GPIOA, LED1_PIN_BIT);
    LED_OFF(GPIOB, LED2_PIN_BIT);
    LED_OFF(GPIOC, LED3_PIN_BIT);

    // Массив для удобного перебора LED
    typedef struct { GPIO_TypeDef* port; uint8_t pin_bit; } LedDef_t;
    LedDef_t leds[] = {
        {GPIOA, LED1_PIN_BIT}, // LED 1 (PA1)
        {GPIOB, LED2_PIN_BIT}, // LED 2 (PB1)
        {GPIOC, LED3_PIN_BIT}  // LED 3 (PC2)
    };
    const uint8_t total_leds = sizeof(leds) / sizeof(LedDef_t);

    // 2. Включаем 'active_led_count' светодиодов, начиная с 'current_led_index'
    for (int i = 0; i < active_led_count; i++) {
        uint8_t idx = (current_led_index + i) % total_leds;
        LED_ON(leds[idx].port, leds[idx].pin_bit);
    }
}

// --- Обработчики событий кнопок ---

// Кнопка 1 (PC0) - Переключение одного светодиода (Task 1)
void on_button1_rise(void* context) {
    (void)context; // Не используется

    // 1. Устанавливаем режим: только один светодиод активен
    // active_led_count = 1;
    
    // 2. Переключаем индекс (0 -> 1 -> 2 -> 0)
    current_led_index = (current_led_index + 1) % 3;
    
    update_leds();
}

// Кнопка 2 (PC1) - Установка количества активных светодиодов (Task 2)
void on_button2_rise(void* context) {
    (void)context; // Не используется
    
    // Переключаем количество активных LED (1 -> 2 -> 3 -> 1)
    active_led_count = (active_led_count % 3) + 1;

    // Сбрасываем индекс, чтобы всегда начинать с LED1 при смене режима
    current_led_index = 0; 

    update_leds();
}


int main(){
    // Инициализация периферии (как в вашем коде)
    SystemClock_Config();
    SysTick_Init();

    GPIOA_Init_Direct();
    GPIOB_Init_Macro();
    GPIOC_Init_CMSIS();
    
    // Инициализация модуля Debouncer
    // Delta T = 1.0 мс, Cutoff Freq = 5 Гц, Hysteresis = 0.8/0.2.

    // Кнопка 1 (PC0)
    Button_Init(
        &btn1_sequence, 
        1.0f, 5.0f, 0.8f, 0.2f, 
        (READ_BIT(GPIOC->IDR, GPIO_IDR_ID0) ? 1 : 0) // Начальное состояние
    );
    Button_SetRiseCallback(&btn1_sequence, on_button1_rise);
    Button_SetContext(&btn1_sequence, &btn1_sequence);

    // Кнопка 2 (PC1)
    Button_Init(
        &btn2_count, 
        1.0f, 5.0f, 0.8f, 0.2f, 
        (READ_BIT(GPIOC->IDR, GPIO_IDR_ID1) ? 1 : 0) // Начальное состояние
    );
    Button_SetRiseCallback(&btn2_count, on_button2_rise);
    Button_SetContext(&btn2_count, &btn2_count);

    update_leds();

    while (1)
    {
    }
}

/**
 * @brief Обработчик прерывания SysTick (1 кГц)
 */
void SysTick_Handler(void) {
    millis++;

    // 1. Считываем сырые состояния кнопок (pull-down: 1 = нажато)
    uint8_t raw_state_btn1 = (READ_BIT(GPIOC->IDR, GPIO_IDR_ID0) ? 1 : 0);
    uint8_t raw_state_btn2 = (READ_BIT(GPIOC->IDR, GPIO_IDR_ID1) ? 1 : 0);

    // 2. Обрабатываем каждое состояние кнопки
    Button_Process(&btn1_sequence, raw_state_btn1);
    Button_Process(&btn2_count, raw_state_btn2);
}