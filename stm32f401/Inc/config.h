// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"
#include "../src/Lighting/LampArrayHidStructs.h"

// Timing
#define LAMPARRAY_UPDATE_INTERVAL   10000                               // 10 ms update interval, in us
#define LAMP_UPDATE_RATE_MS         LAMPARRAY_UPDATE_INTERVAL / 1000
#define KEY_POLLING_RATE            10                                  // 10 ms polling rate

// Rotary Encoder Pins (using GPIOA)
#define ROTA_PIN                    GPIO_PIN_0
#define ROTA_PORT                   GPIOA
#define ROTB_PIN                    GPIO_PIN_1
#define ROTB_PORT                   GPIOA
#define ROT_BTN_PIN                 GPIO_PIN_2
#define ROT_BTN_PORT                GPIOA

// WS2812 Configuration
#define WS2812_PIN                  GPIO_PIN_8
#define WS2812_PORT                 GPIOA
#define WS2812_TIMER                TIM1
#define WS2812_TIMER_CHANNEL        TIM_CHANNEL_1
#define NEOPIXEL_COUNT              12

// Key GPIO Pins (using GPIOB)
#define KEY1_PIN                    GPIO_PIN_0
#define KEY2_PIN                    GPIO_PIN_1
#define KEY3_PIN                    GPIO_PIN_2
#define KEY4_PIN                    GPIO_PIN_3
#define KEY5_PIN                    GPIO_PIN_4
#define KEY6_PIN                    GPIO_PIN_5
#define KEY7_PIN                    GPIO_PIN_6
#define KEY8_PIN                    GPIO_PIN_7
#define KEY9_PIN                    GPIO_PIN_8
#define KEY10_PIN                   GPIO_PIN_9
#define KEY11_PIN                   GPIO_PIN_10
#define KEY12_PIN                   GPIO_PIN_11
#define KEY_PORT                    GPIOB

// Key Indices
#define KEY1                        0
#define KEY2                        1
#define KEY3                        2
#define KEY4                        3
#define KEY5                        4
#define KEY6                        5
#define KEY7                        6
#define KEY8                        7
#define KEY9                        8
#define KEY10                       9
#define KEY11                       10
#define KEY12                       11

// LampArray Attributes
#define LAMPARRAY_LAMP_COUNT        NEOPIXEL_COUNT
#define LAMPARRAY_WIDTH             59000    // 5.9 cm 
#define LAMPARRAY_HEIGHT            104000   // 10.4 cm
#define LAMPARRAY_DEPTH             25000    // 2.5 cm (including rotary encoder)
#define LAMPARRAY_KIND              1        // LampArrayKindKeyboard

// Lamp Positions
static Position LampPositions[NEOPIXEL_COUNT] = {
    {10500, 30000, 0}, {29500, 30000, 0}, {48500, 30000, 0}, 
    {10500, 49500, 0}, {29500, 49500, 0}, {48500, 49500, 0}, 
    {10500, 69000, 0}, {29500, 69000, 0}, {48500, 69000, 0}, 
    {10500, 88500, 0}, {29500, 88500, 0}, {48500, 88500, 0}
};

// Helper function
static inline uint16_t LampIdToKey(uint16_t lampId) {
    if (lampId >= LAMPARRAY_LAMP_COUNT) {
        return LAMPARRAY_LAMP_COUNT - 1;
    }
    return lampId;
}

#define MAX_KEYCODES                6

// Misc.
#define AUTONOMOUS_LIGHTING_COLOR   (LampColor){0, 255, 0}
#define AUTONOMOUS_LIGHTING_EFFECT  BLINK

#endif // CONFIG_H
