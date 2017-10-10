#include "debug_hw.h"

#ifdef RELEASE
void Debug_HW_Init( void ){ return; }
void Debug_HW_Deinit( void ){ return; }
void Debug_HW_set( uint8_t chan, uint8_t state ){ return; }
void Debug_HW_toggle( uint8_t chan ){ return; }
#endif // RELEASE

#ifdef DEBUG
void Debug_HW_Init( void )
{
	DBG_HW_GPIO_RCC();

	GPIO_InitTypeDef gpio;
	gpio.Pin  = DBG_HW_BM_0
	          | DBG_HW_BM_1
			  | DBG_HW_LED_X
			  | DBG_HW_LED_Y
			  ;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init( DBG_HW_GPIO, &gpio );
}
void Debug_HW_Deinit( void )
{
	DBG_HW_GPIO_RCC_D();
}

const uint8_t ch[4] = { DBG_HW_BM_0
                      , DBG_HW_BM_1
					  , DBG_HW_LED_X
					  , DBG_HW_LED_Y
					  };
void Debug_HW_set( uint8_t chan, uint8_t state )
{
	const uint8_t st[2] = { GPIO_PIN_RESET
	                      , GPIO_PIN_SET
						  };
	HAL_GPIO_WritePin( DBG_HW_GPIO
	                 , ch[chan]
					 , st[state]
					 );
}

void Debug_HW_toggle( uint8_t chan )
{
	HAL_GPIO_TogglePin( DBG_HW_GPIO, ch[chan] );
}
#endif // DEBUG
