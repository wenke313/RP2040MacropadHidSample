// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// Modified for STM32F401

#include "stm32f4xx_hal.h"
#include "tusb.h"
#include "config.h"
#include "neopixel_stm32.h"
#include "keys_stm32.h"
#include "encoder_stm32.h"
#include "../../src/Lighting/LampArray.h"
#include "../../src/Lighting/LampArrayHidStructs.h"
#include "../../src/usb_descriptors.h"

// Hardware handles
TIM_HandleTypeDef htim1;  // For WS2812
TIM_HandleTypeDef htim2;  // For general timing

// Function prototypes
void SystemClock_Config(void);
void GPIO_Init(void);
void TIM1_Init(void);  // WS2812 PWM timer
void TIM2_Init(void);  // General purpose timer
void Error_Handler(void);

// Timer callback flags
volatile uint8_t lightingUpdateFlag = 0;
volatile uint8_t keyboardUpdateFlag = 0;

int main(void) {
    // Initialize HAL
    HAL_Init();
    
    // Configure system clock
    SystemClock_Config();
    
    // Initialize peripherals
    GPIO_Init();
    TIM1_Init();  // WS2812 timer
    TIM2_Init();  // General timer for callbacks
    
    // Initialize USB
    tusb_init();
    
    // Initialize application modules
    initKeys();
    initEncoder();
    NeopixelInit(&htim1, AUTONOMOUS_LIGHTING_EFFECT, AUTONOMOUS_LIGHTING_COLOR);
    
    // Start timer for periodic updates
    HAL_TIM_Base_Start_IT(&htim2);
    
    // Main loop
    while (1) {
        // Handle USB tasks
        tud_task();
        
        // Handle lighting updates (10ms interval)
        if (lightingUpdateFlag) {
            lightingUpdateFlag = 0;
            NeopixelUpdateEffect();
        }
        
        // Handle keyboard polling (10ms interval)
        if (keyboardUpdateFlag) {
            keyboardUpdateFlag = 0;
            
            // Remote wakeup
            if (tud_suspended()) {
                tud_remote_wakeup();
            } 
            else if (tud_hid_ready()) {
                sendKeyboardReport();
            }
        }
    }
}

/**
 * @brief System Clock Configuration
 * Configure system to run at 84 MHz from HSE (25MHz crystal)
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable Power Control clock
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    // Initialize HSE oscillator and PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;  // 84 MHz
    RCC_OscInitStruct.PLL.PLLQ = 7;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initialize CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization
 */
void GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure WS2812 pin (PA8) - will be configured by TIM1
    
    // Configure key pins (PB0-PB11) with pull-up
    GPIO_InitStruct.Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN |
                         KEY5_PIN | KEY6_PIN | KEY7_PIN | KEY8_PIN |
                         KEY9_PIN | KEY10_PIN | KEY11_PIN | KEY12_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);

    // Configure rotary encoder pins with pull-up
    GPIO_InitStruct.Pin = ROTB_PIN | ROT_BTN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure ROTA with interrupt on falling edge
    GPIO_InitStruct.Pin = ROTA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ROTA_PORT, &GPIO_InitStruct);

    // Enable EXTI interrupt
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
 * @brief TIM1 Initialization for WS2812 PWM
 * Timer configured for 800kHz with variable duty cycle
 */
void TIM1_Init(void) {
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // Enable TIM1 clock
    __HAL_RCC_TIM1_CLK_ENABLE();

    // TIM1 configuration
    // Clock: 84 MHz (APB2)
    // Target: 800 kHz bit rate, 1.25us per bit
    // PWM period: 105 counts (84MHz / 800kHz = 105)
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 104;  // 105 counts - 1
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }

    // PWM channel configuration
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    // Master configuration
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    // Configure GPIO for TIM1 CH1 (PA8)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = WS2812_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(WS2812_PORT, &GPIO_InitStruct);
}

/**
 * @brief TIM2 Initialization for periodic callbacks
 * Configured for 10ms interrupts
 */
void TIM2_Init(void) {
    // Enable TIM2 clock
    __HAL_RCC_TIM2_CLK_ENABLE();

    // TIM2 configuration for 10ms (100Hz) interrupts
    // Clock: 42 MHz (APB1 * 2)
    // Prescaler: 4200 - 1 -> 10 kHz
    // Period: 100 - 1 -> 100 Hz (10ms)
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 4199;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 99;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }

    // Enable TIM2 interrupt
    HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief TIM2 Interrupt Handler
 */
void TIM2_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&htim2, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
            
            // Set flags for main loop processing
            lightingUpdateFlag = 1;
            keyboardUpdateFlag = 1;
        }
    }
}

/**
 * @brief EXTI0 Interrupt Handler (Rotary Encoder A)
 */
void EXTI0_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(ROTA_PIN) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(ROTA_PIN);
        Encoder_IRQHandler();
    }
}

/**
 * @brief Error Handler
 */
void Error_Handler(void) {
    __disable_irq();
    while (1) {
        // Error loop
    }
}

//--------------------------------------------------------------------
// TinyUSB HID Callbacks
//--------------------------------------------------------------------

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(
        uint8_t instance,
        uint8_t report_id,
        hid_report_type_t report_type,
        uint8_t* buffer,
        uint16_t reqlen) {
    switch (report_id) {
        case REPORT_ID_LIGHTING_LAMP_ARRAY_ATTRIBUTES: {
            return GetLampArrayAttributesReport(buffer);
        }
        case REPORT_ID_LIGHTING_LAMP_ATTRIBUTES_RESPONSE: { 
            return GetLampAttributesReport(buffer);
        }
        default: {
            break;
        }
    }
    return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(
        uint8_t instance,
        uint8_t report_id,
        hid_report_type_t report_type,
        uint8_t const* buffer,
        uint16_t bufsize) {
    switch (report_id) {
        case REPORT_ID_LIGHTING_LAMP_ATTRIBUTES_REQUEST: {
            SetLampAttributesId(buffer);
            break;
        }
        case REPORT_ID_LIGHTING_LAMP_MULTI_UPDATE: {
            SetMultipleLamps(buffer);
            break;   
        }
        case REPORT_ID_LIGHTING_LAMP_RANGE_UPDATE: {
            SetLampRange(buffer);
            break;   
        }
        case REPORT_ID_LIGHTING_LAMP_ARRAY_CONTROL: {
            SetAutonomousMode(buffer);
            break;
        }
        default: {
            break;
        }
    }
}

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {}

//--------------------------------------------------------------------
// TinyUSB Device callbacks
//--------------------------------------------------------------------

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {}
void tud_mount_cb(void) {}
void tud_unmount_cb(void) {}
void tud_suspend_cb(bool remote_wakeup_en) {}
void tud_resume_cb(void) {}

//--------------------------------------------------------------------
// HAL MSP Functions
//--------------------------------------------------------------------

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
    if (htim_base->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm) {
    if (htim_pwm->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();
        
        // Configure DMA for TIM1
        // DMA2 Stream 5 Channel 6 for TIM1_CH1
        static DMA_HandleTypeDef hdma_tim1_ch1;
        hdma_tim1_ch1.Instance = DMA2_Stream5;
        hdma_tim1_ch1.Init.Channel = DMA_CHANNEL_6;
        hdma_tim1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim1_ch1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim1_ch1.Init.Mode = DMA_NORMAL;
        hdma_tim1_ch1.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_tim1_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        
        if (HAL_DMA_Init(&hdma_tim1_ch1) != HAL_OK) {
            Error_Handler();
        }
        
        __HAL_LINKDMA(htim_pwm, hdma[TIM_DMA_ID_CC1], hdma_tim1_ch1);
    }
}
