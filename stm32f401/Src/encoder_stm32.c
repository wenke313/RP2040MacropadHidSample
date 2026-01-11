// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#include "encoder_stm32.h"
#include "keys_stm32.h"
#include "config.h"

static volatile uint32_t last_interrupt_time = 0;

void initEncoder(void) {
    // GPIO configuration is done in main.c via HAL_GPIO_Init
    // ROTA and ROTB are configured with pull-up resistors
    // ROTA is configured for interrupt on falling edge
}

// This function is called from the EXTI interrupt handler
void Encoder_IRQHandler(void) {
    uint32_t current_time = HAL_GetTick();
    
    // Debounce: ignore interrupts within 50ms
    if (current_time - last_interrupt_time < 50) {
        return;
    }
    last_interrupt_time = current_time;
    
    // Read encoder direction
    if (HAL_GPIO_ReadPin(ROTB_PORT, ROTB_PIN) == GPIO_PIN_SET) {
        // Counter clockwise
        updateCurrentLayer(-1);
    }
    else {
        // Clockwise
        updateCurrentLayer(1);
    }
}
