/*****************************************************************************
 *
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef AMD_SPI_HC_NV_DATA_H_
#define AMD_SPI_HC_NV_DATA_H_

#include <Guid/HiiPlatformSetupFormset.h>

#define AMD_SPI_HC_FORMSET_GUID \
  { \
    0x64C1CC5F, 0xED4C, 0x4042, {0x94, 0x55, 0xD3, 0x85, 0x9B, 0x2F, 0x78, 0xD8} \
  }

extern EFI_GUID mAmdSpiHcFormSetGuid;
extern CHAR16  mAmdSpiHcNvDataVar[];

#define AMD_SPI_HC_NV_DATA_VARIABLE L"AMD_SPI_HC_NV_DATA"
#define AMD_SPI_HC_VARSTORE_ID  0x0001

#define KEY_ROM_ARMOR_ENABLE            0x2000
#define KEY_ROM_ARMOR_ALLOWLIST_ENABLE  0x2001

#pragma pack(1)

typedef struct _AMD_SPI_HC_NV_DATA {
  UINT8 RomArmorEnable;
  UINT8 AllowlistEnable;
} AMD_SPI_HC_NV_DATA;

#pragma pack()

#endif // AMD_SPI_HC_NV_DATA_H_
