#include "usbd_main.h"

#include "../lib/debug_usart.h"

USBD_HandleTypeDef USBD_Device;

void usbd_main(void)
{
  // Init Device Library
  USBD_Init(&USBD_Device, &DFU_Desc, 0);
  
  // Add Supported Class
  USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS);
  
  // Add Interface callbacks for AUDIO Class
  USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_Flash_fops);
  
  // Start Device Process
  USBD_Start(&USBD_Device);
}

extern PCD_HandleTypeDef hpcd;

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}
