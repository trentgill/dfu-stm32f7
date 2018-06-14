#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/debug_usart.h"
#include "lib/debug_pin.h"
#include "usbd/usbd_main.h"

// private fn prototypes
static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

uint32_t kStartExecutionAddress = 0x08010000;

typedef void (*pFunc)(void);
void JumpTo(uint32_t address) {
	// deinit USB here!!!

	// Deinit open drivers
	//Debug_HW_Deinit();
	Debug_USART_DeInit();
	//ONE_HW_Deinit();

	HAL_DeInit();

    // Reinitialize the Stack pointer
    __set_MSP(*(__IO uint32_t*) address);
    // jump to application address
    ((pFunc) (*(__IO uint32_t*) (address + 4)))();
    
    while(1){}
}

// exported fns
int main(void)
{
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

// check if USB-cable present
	
// wait for bootloader to finish
    uint8_t tog;
	while( 1 ){ // bootload in IRQ
        Debug_Pin_Set(tog); tog ^= 1;
	}

	JumpTo(kStartExecutionAddress);

	while(1){} // wait for jump
    return 0;  // won't reach here
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
