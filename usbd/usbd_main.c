#include "usbd_main.h"

#include "../lib/debug_usart.h"

USBD_HandleTypeDef USBD_Device;

void USB_DFU_Init(void)
{
    // Init Device Library
    USBD_Init( &USBD_Device, &DFU_Desc, 0 );

    // Add Supported Class
    USBD_RegisterClass( &USBD_Device, USBD_DFU_CLASS );

    // Add Interface callbacks for DFU Class
    USBD_DFU_RegisterMedia( &USBD_Device, &USBD_DFU_Flash_fops );

    // Start Device Process
    USBD_Start( &USBD_Device );
}

void USB_DFU_Deinit(void)
{
    USBD_Stop( &USBD_Device );
    USBD_DeInit( &USBD_Device );
}

extern volatile uint8_t usb_is_connected;

uint8_t USB_DFU_ongoing(void)
{
    if( !usb_is_connected ){ return 0; }
    // timeout?
    // upload finished?
    // upload failed? -> should retry?
    //

    return 1;
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
