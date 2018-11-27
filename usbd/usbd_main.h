#pragma once

#include "stm32f7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dfu.h"
#include "usbd_dfu_flash.h"

#define USE_USB_FS

void USB_DFU_Init(void);
void USB_DFU_Deinit(void);

uint8_t USB_DFU_ongoing(void);
