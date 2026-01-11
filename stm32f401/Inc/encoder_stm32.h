// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#ifndef ENCODER_STM32_H
#define ENCODER_STM32_H

#include "stm32f4xx_hal.h"

void initEncoder(void);
void Encoder_IRQHandler(void);

#endif // ENCODER_STM32_H
