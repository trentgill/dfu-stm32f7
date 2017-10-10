#include "usbd_main.h"

#include "../lib/debug_usart.h"

USBD_HandleTypeDef USBD_Device;

void usbd_main(void)
{
  // Init Device Library
  USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
  
  // Add Supported Class
  USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
  
  // Add Interface callbacks for AUDIO Class
  USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);
  
  // Start Device Process
  USBD_Start(&USBD_Device);
}

extern PCD_HandleTypeDef hpcd;
// extern SAI_HandleTypeDef haudio_out_sai;

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}
