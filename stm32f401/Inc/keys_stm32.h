// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#ifndef KEYS_STM32_H
#define KEYS_STM32_H

#include "stm32f4xx_hal.h"

void initKeys(void);
void sendKeyboardReport(void);
void updateCurrentLayer(int amount);

#endif // KEYS_STM32_H
