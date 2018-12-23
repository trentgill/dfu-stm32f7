#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/debug_usart.h"
#include "lib/debug_pin.h"
#include "usbd/usbd_main.h"

#define GOTO_BOOT_ADDRESS  0x20006666
#define GOTO_BOOT_MAGICNUM 0x66666666
#define EXEC_ADDRESS       0x08010000

// private fn prototypes
static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

typedef void (*pFunc)(void);
static void JumpTo(uint32_t address)
{
    // Reinitialize the Stack pointer
    __set_MSP(*(__IO uint32_t*) address);
    // jump to application address
    ((pFunc) (*(__IO uint32_t*) (address + 4)))();
    while(1){}
}

#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
uint8_t I2C_is_boot( void )
{
    uint8_t boot = 0;
	GPIO_InitTypeDef gpio;
	I2Cx_SCL_GPIO_CLK_ENABLE();
	gpio.Pin       = I2Cx_SCL_PIN;
	gpio.Mode      = GPIO_MODE_INPUT;
	gpio.Pull      = GPIO_PULLUP;
	gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( I2Cx_SCL_GPIO_PORT, &gpio );

    if( !HAL_GPIO_ReadPin( I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN ) ){
        boot = 1;
    }

    // set to OD to ensure no damage by i2c line
    // FIXME is this necessary, or does DeInit set the pin to tristate?
	gpio.Mode      = GPIO_MODE_AF_OD;
	HAL_GPIO_Init( I2Cx_SCL_GPIO_PORT, &gpio );

	HAL_GPIO_DeInit( I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN );
    return boot;
}

// exported fns
int main(void)
{
    if( !I2C_is_boot() ){
        // Check if magic RAM location is set
        if( *(uint32_t*)(GOTO_BOOT_ADDRESS) != GOTO_BOOT_MAGICNUM ){
            JumpTo(EXEC_ADDRESS);
        }
    }

    // If we're bootloading, first unset the bit
    *(uint32_t*)(GOTO_BOOT_ADDRESS) = 0;

	// Configure low-level
	MPU_Config();
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();

	// HW initialization
    Debug_Pin_Init();
    Debug_USART_Init();
	U_PrintLn("dfu bootload");
    U_PrintNow();

// init USB
	USB_DFU_Init();

    while( USB_DFU_ongoing() ){
        HAL_Delay(1000); U_PrintLn("."); U_PrintNow();
    }

// deinit hardware layer
    USB_DFU_Deinit();
	Debug_USART_DeInit();
	HAL_DeInit();

	JumpTo(EXEC_ADDRESS);

    return 0;
}

// LOW LEVEL SYS INIT
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 432;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 9;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){ Error_Handler(); }

    if(HAL_PWREx_EnableOverDrive() != HAL_OK) { Error_Handler(); }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_HCLK
                                | RCC_CLOCKTYPE_PCLK1
                                | RCC_CLOCKTYPE_PCLK2
                                ;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK){
        Error_Handler();
    }
}

static void Error_Handler(void)
{
	while(1) {
		;;
	}
}

static void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;
	
	// Disable the MPU
	HAL_MPU_Disable();

	// Configure the MPU attributes as WT for SRAM
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x20020000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// Enable the MPU
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void)
{
	// Enable I-Cache
	SCB_EnableICache();

	// Enable D-Cache
	SCB_EnableDCache();
}

#ifdef  USE_FULL_ASSERT
// Reports the name of the source file and the source line number
void assert_failed(uint8_t* file, uint32_t line)
{ 
	// printf the failure: "error in %file at line %line"
	// Infinite loop (just freezing now)
	while (1) {
		;;
	}
}
#endif
