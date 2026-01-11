// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401 - WS2812 Driver using PWM+DMA

#include "ws2812_stm32.h"
#include <string.h>

// DMA buffer for PWM data
static uint16_t ws2812_buffer[WS2812_BUFFER_SIZE];
static uint8_t pixel_buffer[NEOPIXEL_COUNT * 3];  // RGB data for each pixel
static TIM_HandleTypeDef *timer_handle;

/**
 * @brief Initialize WS2812 driver
 * @param htim Timer handle (should be pre-configured for PWM)
 */
void WS2812_Init(TIM_HandleTypeDef *htim) {
    timer_handle = htim;
    
    // Clear all pixels
    memset(pixel_buffer, 0, sizeof(pixel_buffer));
    memset(ws2812_buffer, 0, sizeof(ws2812_buffer));
    
    // Initialize PWM
    HAL_TIM_PWM_Start_DMA(timer_handle, WS2812_TIMER_CHANNEL, 
                          (uint32_t*)ws2812_buffer, WS2812_BUFFER_SIZE);
}

/**
 * @brief Set individual pixel color
 * @param index Pixel index (0 to NEOPIXEL_COUNT-1)
 * @param r Red value (0-255)
 * @param g Green value (0-255)
 * @param b Blue value (0-255)
 */
void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= NEOPIXEL_COUNT) {
        return;
    }
    
    // WS2812B uses GRB order
    pixel_buffer[index * 3 + 0] = g;
    pixel_buffer[index * 3 + 1] = r;
    pixel_buffer[index * 3 + 2] = b;
}

/**
 * @brief Set pixel color using 32-bit color value
 * @param index Pixel index
 * @param color Color in 0x00RRGGBB format
 */
void WS2812_SetPixelColor(uint16_t index, uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    WS2812_SetPixel(index, r, g, b);
}

/**
 * @brief Clear all pixels (set to black)
 */
void WS2812_Clear(void) {
    memset(pixel_buffer, 0, sizeof(pixel_buffer));
}

/**
 * @brief Update WS2812 LEDs with current pixel buffer
 * Converts RGB data to PWM timings and triggers DMA transfer
 */
void WS2812_Update(void) {
    uint32_t buffer_index = 0;
    
    // Convert pixel data to PWM values
    for (uint16_t i = 0; i < NEOPIXEL_COUNT * 3; i++) {
        uint8_t byte = pixel_buffer[i];
        
        // Convert each bit to PWM value (MSB first)
        for (int8_t bit = 7; bit >= 0; bit--) {
            if (byte & (1 << bit)) {
                ws2812_buffer[buffer_index] = WS2812_1_CODE;  // Logic 1
            } else {
                ws2812_buffer[buffer_index] = WS2812_0_CODE;  // Logic 0
            }
            buffer_index++;
        }
    }
    
    // Add reset pulse (50us low = 40 bits at 800kHz)
    for (uint16_t i = 0; i < 50; i++) {
        ws2812_buffer[buffer_index++] = 0;
    }
    
    // Start DMA transfer
    HAL_TIM_PWM_Start_DMA(timer_handle, WS2812_TIMER_CHANNEL, 
                          (uint32_t*)ws2812_buffer, buffer_index);
}
