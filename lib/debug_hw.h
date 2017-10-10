#pragma once

#include <stm32f7xx.h>

#define DBG_HW_GPIO_RCC()		__HAL_RCC_GPIOA_CLK_ENABLE()

#define DBG_HW_GPIO				GPIOA
#define DBG_HW_BM_0				GPIO_PIN_4
#define DBG_HW_BM_1				GPIO_PIN_5
#define DBG_HW_LED_X			GPIO_PIN_6
#define DBG_HW_LED_Y			GPIO_PIN_7

void Debug_HW_Init( void );
void Debug_HW_set( uint8_t chan, uint8_t state );
void Debug_HW_toggle( uint8_t chan );
