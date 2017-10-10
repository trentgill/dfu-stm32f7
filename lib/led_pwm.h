#pragma once

#include <stm32f7xx.h>

#define ONE_LED_GPIO_RCC()		__HAL_RCC_GPIOD_CLK_ENABLE()
#define ONE_LED_GPIO_RCC_D()	__HAL_RCC_GPIOD_CLK_DISABLE()
#define ONE_LED_GPIO			GPIOD
#define ONE_LED_MOTOR			GPIO_PIN_9
#define ONE_LED_TOUCH			GPIO_PIN_11
#define ONE_LED_CHALK			GPIO_PIN_4
#define ONE_LED_OVER			GPIO_PIN_5

#define ONE_MODE_LED_GPIO_RCC()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define ONE_MODE_LED_GPIO_RCC_D()	__HAL_RCC_GPIOB_CLK_DISABLE()
#define ONE_MODE_LED_GPIO		GPIOB
#define ONE_MODE_LED			GPIO_PIN_13

#define PWM_LEVELS 32

typedef enum
	{ MOTOR_L
	, TOUCH_L
	, CHALK_L
	, OVER_L
	, MODE_L
	, PWM_COUNT // < length of the enum
	} ONE_led_t;

void PWM_Init( void );
void PWM_Deinit( void );
void PWM_set_level( ONE_led_t ch, float level );
void PWM_step( void );