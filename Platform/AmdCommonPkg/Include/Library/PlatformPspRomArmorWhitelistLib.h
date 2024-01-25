/*****************************************************************************
 *
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/**
 * @file
 *
 * Platform ROM Armor Whitelist table
 *
 */

#include <Library/AmdPspRomArmorLib.h>
/*
 *  Return allocated and filled AMD PSP ROM Armor White list Table
 *
 *
 * @param[in]  PlatformSpiWhitelist   Pointer to white list table
 *
 * @return    EFI_SUCCESS
 * @return    EFI_OUT_OF_RESOURCES      Buffer to return couldn't be allocated
 */
EFI_STATUS
EFIAPI
GetPspRomArmorWhitelist (
  IN       SPI_WHITE_LIST     **PlatformSpiWhitelist
  );
