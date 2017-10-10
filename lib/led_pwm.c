#include "led_pwm.h"

#include "wrMath.h"

uint8_t pwm_states[PWM_COUNT]; // leds

void PWM_Init( void )
{
	ONE_LED_GPIO_RCC();
	GPIO_InitTypeDef gpio;
	gpio.Pin = ONE_LED_OVER | ONE_LED_CHALK | ONE_LED_MOTOR | ONE_LED_TOUCH;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ONE_LED_GPIO, &gpio);

	// mode LED
	ONE_MODE_LED_GPIO_RCC();
    gpio.Pin = ONE_MODE_LED;
	HAL_GPIO_Init(ONE_MODE_LED_GPIO, &gpio);

	for( uint8_t i=0; i<PWM_COUNT; i++ ){
		pwm_states[i] = 0;
	}
}
void PWM_Deinit( void )
{
	ONE_LED_GPIO_RCC_D();
	ONE_MODE_LED_GPIO_RCC_D();
}

void PWM_set_level( ONE_led_t ch, float level )
{
	const float pwm_max = (float)PWM_LEVELS;

	pwm_states[ch] = (uint8_t)(lim_f_0_1( level ) * pwm_max);
}

void ll_led_set( uint8_t ch, uint8_t state );

void PWM_step( void )
{
	static uint8_t ix;

	for( uint8_t i=0; i<PWM_COUNT; i++ ){
		// refactor to only call change on state change
		ll_led_set( i
			      , (pwm_states[i] > ix)
			      );
	}
	ix++; if(ix >= PWM_LEVELS){ ix = 0; } // next step
}

void ll_led_set( uint8_t ch, uint8_t state )
{
	const uint8_t st[2] = {GPIO_PIN_RESET, GPIO_PIN_SET};
	switch(ch) {
		case MOTOR_L:
			HAL_GPIO_WritePin( ONE_LED_GPIO
	                         , ONE_LED_MOTOR
					         , st[state]
					         );
			break;
		case TOUCH_L:
			HAL_GPIO_WritePin( ONE_LED_GPIO
	                         , ONE_LED_TOUCH
					         , st[state]
					         );
			break;
		case CHALK_L:
			HAL_GPIO_WritePin( ONE_LED_GPIO
	                         , ONE_LED_CHALK
					         , st[state]
					         );
			break;
		case OVER_L:
			HAL_GPIO_WritePin( ONE_LED_GPIO
	                         , ONE_LED_OVER
					         , st[state]
					         );
			break;
		case MODE_L:
			HAL_GPIO_WritePin( ONE_MODE_LED_GPIO
	                         , ONE_MODE_LED
					         , st[state]
					         );
			break;
	}
	
}