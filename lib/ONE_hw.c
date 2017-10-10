#include "ONE_hw.h"
#include "debug_usart.h"
#include "debug_hw.h"
#include "led_pwm.h"

// private declarations
typedef enum
    { MOTOR_B = ONE_PUSH_MOTOR
	, TOUCH_B = ONE_PUSH_TOUCH
	, CHALK_B  = ONE_PUSH_CHALK
	} ONE_button_t;

typedef enum
    { UP_TOG   = ONE_TOG_UP
	, DOWN_TOG = ONE_TOG_DOWN
	} ONE_toggle_t;

typedef enum
    { IN_D  = ONE_DETECT_IN
	, OUT_D = ONE_DETECT_OUT
	, CV_D  = ONE_DETECT_CV
	} ONE_detect_t;

uint8_t _getbutton( ONE_button_t b );
uint8_t _gettoggle( ONE_toggle_t t );
uint8_t _getdetect( ONE_detect_t d );

// public defns
void ONE_HW_Init(void)
{
	PWM_Init();

	GPIO_InitTypeDef gpio;

	// Buttons
	ONE_PUSH_GPIO_RCC();
	gpio.Pin = ONE_PUSH_MOTOR | ONE_PUSH_TOUCH | ONE_PUSH_CHALK;
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(ONE_PUSH_GPIO, &gpio);

	// Toggle
	ONE_TOG_GPIO_RCC();
	gpio.Pin = ONE_TOG_UP | ONE_TOG_DOWN;
	HAL_GPIO_Init(ONE_TOG_GPIO, &gpio);

	// Jack Sense
	ONE_DETECT_GPIO_RCC();
	gpio.Pin = ONE_DETECT_IN | ONE_DETECT_OUT | ONE_DETECT_CV;
	HAL_GPIO_Init(ONE_DETECT_GPIO, &gpio);

	// Power Sense
	ONE_SENSE_GPIO_RCC();
	gpio.Pin = ONE_SENSE_PIN;
	gpio.Mode = GPIO_MODE_IT_FALLING;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ONE_SENSE_GPIO, &gpio);
	HAL_NVIC_SetPriority(ONE_SENSE_IRQn, 0, 0); // HIGHEST PRIORITY!!
	HAL_NVIC_EnableIRQ(ONE_SENSE_IRQn);

}

// States
uint8_t _getbutton( ONE_button_t b )
{
    return HAL_GPIO_ReadPin( ONE_PUSH_GPIO
	                       , b
						   );
}
uint8_t _gettoggle( ONE_toggle_t t )
{ // nb: active low
    return !HAL_GPIO_ReadPin( ONE_TOG_GPIO
	                        , t
						    );
}
uint8_t _getdetect( ONE_detect_t d )
{ // nb: active low
    return !HAL_GPIO_ReadPin( ONE_DETECT_GPIO
	                        , d
						    );
}
// Returns a *change* flag, and overwrites *mask
uint8_t ONE_getstates( uint8_t* mask )
{
	uint8_t prev = *mask;
	*mask = 0; // clear state

	*mask |= _getbutton( MOTOR_B );
	*mask |= _getbutton( TOUCH_B ) << 1;
	*mask |= _getbutton( CHALK_B ) << 2;

	*mask |= _gettoggle( UP_TOG   ) << 3;
	*mask |= _gettoggle( DOWN_TOG ) << 4;

	// *mask |= _getdetect( IN_D  ) << 5;
	// *mask |= _getdetect( OUT_D ) << 6;
	// *mask |= _getdetect( CV_D  ) << 7;

	return (*mask != prev ); // 1 if state has changed
}
uint8_t ONE_getdetects( uint8_t* mask )
{
	uint8_t prev = *mask;
	*mask = 0; // clear state

	// *mask |= _getbutton( MOTOR_B );
	// *mask |= _getbutton( TOUCH_B ) << 1;
	// *mask |= _getbutton( CHALK_B ) << 2;

	// *mask |= _gettoggle( UP_TOG   ) << 3;
	// *mask |= _gettoggle( DOWN_TOG ) << 4;

	*mask |= _getdetect( IN_D  ) << 5;
	*mask |= _getdetect( OUT_D ) << 6;
	*mask |= _getdetect( CV_D  ) << 7;

	return (*mask != prev ); // 1 if state has changed
}


// Power-out Sense action
// Interrupt Handlers
void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(ONE_SENSE_PIN);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // turn off power-sucks
	// finish writing open pages. (w/ linear fadeout?!)
}