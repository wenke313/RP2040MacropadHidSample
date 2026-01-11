// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#ifndef NEOPIXEL_STM32_H
#define NEOPIXEL_STM32_H

#include "stm32f4xx_hal.h"
#include "../../src/Lighting/LampArrayHidStructs.h"

typedef enum {
    HID,
    SOLID,
    BLINK
} NeopixelEffect;

void NeopixelInit(TIM_HandleTypeDef *htim, NeopixelEffect effect, LampColor initialEffectColor);
void NeopixelSetEffect(NeopixelEffect effect, LampColor effectColor);

void NeopixelSetColor(uint16_t lampId, LampColor lampColor);
void NeopixelSetColorRange(uint16_t lampIdStart, uint16_t lampIdEnd, LampColor lampColor);

void NeopixelSendColors(void);
void NeopixelUpdateEffect(void);

#endif // NEOPIXEL_STM32_H
