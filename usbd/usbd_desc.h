#pragma once

#include "usbd_def.h"

#define DEVICE_ID1            (0x1FFF7A10)
#define DEVICE_ID2            (0x1FFF7A14)
#define DEVICE_ID3            (0x1FFF7A18)

#define USB_SIZ_STRING_SERIAL 0x1A

extern USBD_DescriptorsTypeDef DFU_Desc;