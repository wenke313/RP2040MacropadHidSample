// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401 - WS2812 Driver using PWM+DMA

#ifndef WS2812_STM32_H
#define WS2812_STM32_H

#include "stm32f4xx_hal.h"
#include "config.h"

// WS2812 Protocol Timing (in nanoseconds)
// At 84MHz system clock, each timer tick is ~11.9ns
// For 800kHz data rate: 1.25us per bit
#define WS2812_FREQ         800000      // 800kHz
#define WS2812_T0H          350         // 350ns ±150ns
#define WS2812_T0L          900         // 900ns ±150ns
#define WS2812_T1H          900         // 900ns ±150ns
#define WS2812_T1L          350         // 350ns ±150ns

// PWM values for bit encoding
#define WS2812_0_CODE       28          // ~350ns high time
#define WS2812_1_CODE       58          // ~730ns high time
#define WS2812_RESET_CODE   0           // Reset pulse

// Buffer size for DMA
#define WS2812_BUFFER_SIZE  (NEOPIXEL_COUNT * 24 + 50)  // 24 bits per LED + reset

// Function prototypes
void WS2812_Init(TIM_HandleTypeDef *htim);
void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SetPixelColor(uint16_t index, uint32_t color);
void WS2812_Clear(void);
void WS2812_Update(void);

#endif // WS2812_STM32_H
