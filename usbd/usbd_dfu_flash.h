#pragma once

#include "usbd_dfu.h"

// Base address of the Flash sectors (512kB flash, f722)
#define ADDR_FLASH_SECTOR_0  ((uint32_t)0x08000000) // 16 Kbytes
#define ADDR_FLASH_SECTOR_1  ((uint32_t)0x08004000) // 16 Kbytes
#define ADDR_FLASH_SECTOR_2  ((uint32_t)0x08008000) // 16 Kbytes
#define ADDR_FLASH_SECTOR_3  ((uint32_t)0x0800C000) // 16 Kbytes
#define ADDR_FLASH_SECTOR_4  ((uint32_t)0x08010000) // 64 Kbytes
#define ADDR_FLASH_SECTOR_5  ((uint32_t)0x08020000) // 128 Kbytes
#define ADDR_FLASH_SECTOR_6  ((uint32_t)0x08040000) // 128 Kbytes
#define ADDR_FLASH_SECTOR_7  ((uint32_t)0x08060000) // 128 Kbytes

extern USBD_DFU_MediaTypeDef  USBD_DFU_Flash_fops;
