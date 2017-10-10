#pragma once

#include "stm32f7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"
//#include "stm32f769i_eval_audio.h"

#define USE_USB_FS

void usbd_main(void);
