// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#include "keys_stm32.h"
#include "tusb.h"
#include "config.h"
#include "../../src/Keys/keymap.h"
#include "../../src/usb_descriptors.h"

static int8_t currentLayer;

// Helper function to read key state
static inline uint8_t readKey(uint16_t pin) {
    return HAL_GPIO_ReadPin(KEY_PORT, pin) == GPIO_PIN_RESET ? 1 : 0;
}

// Helper function to read rotary button
static inline uint8_t readRotButton(void) {
    return HAL_GPIO_ReadPin(ROT_BTN_PORT, ROT_BTN_PIN) == GPIO_PIN_RESET ? 1 : 0;
}

void initKeys(void) {
    // GPIO configuration is done in main.c via HAL_GPIO_Init
    // Keys are configured with pull-up resistors
    currentLayer = 0;
}

void sendKeyboardReport(void) {
    hid_keyboard_report_t keyState = {0};
    static bool hasSentMacro = false;

    // Check rotary button first
    if (readRotButton()) {
        memcpy(&keyState,
               &layers[currentLayer].macros[0],  // ROT_BTN is index 0
               sizeof(hid_keyboard_report_t));
    }
    // Check all 12 keys
    else if (readKey(KEY1_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY1 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY2_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY2 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY3_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY3 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY4_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY4 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY5_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY5 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY6_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY6 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY7_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY7 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY8_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY8 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY9_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY9 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY10_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY10 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY11_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY11 + 1],
               sizeof(hid_keyboard_report_t));
    }
    else if (readKey(KEY12_PIN)) {
        memcpy(&keyState, 
               &layers[currentLayer].macros[KEY12 + 1],
               sizeof(hid_keyboard_report_t));
    }

    if (keyState.modifier != 0 || keyState.keycode[0] != 0) {
        tud_hid_keyboard_report(
            REPORT_ID_KEYBOARD,
            keyState.modifier,
            keyState.keycode);

        hasSentMacro = true;
    }
    else if (hasSentMacro) {
        // Send empty key report if previously has key pressed
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        hasSentMacro = false;
    }
}

void updateCurrentLayer(int amount) {
    currentLayer += amount;
    currentLayer %= NUM_LAYERS;
}
