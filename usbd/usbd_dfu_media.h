#pragma once

#include "usbd_dfu.h"

// Base address of the Flash sectors
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) // Base @ of Sector 0, 32 Kbytes
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) // Base @ of Sector 1, 32 Kbytes
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) // Base @ of Sector 2, 32 Kbytes
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) // Base @ of Sector 3, 32 Kbytes
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) // Base @ of Sector 4, 128 Kbytes
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) // Base @ of Sector 5, 256 Kbytes
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) // Base @ of Sector 6, 256 Kbytes
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) // Base @ of Sector 7, 256 Kbytes
// below only on 2MB chip
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08100000) // Base @ of Sector 8, 256 Kbytes
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x08140000) // Base @ of Sector 9, 256 Kbytes
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x08180000) // Base @ of Sector 10, 256 Kbytes
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x081C0000) // Base @ of Sector 11, 256 Kbytes


extern USBD_DFU_MediaTypeDef  USBD_DFU_Flash_fops;