/*****************************************************************************
 *
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/**
 * @file
 *
 * Platform ROM Armor Allowlist table
 *
 */

#include <Library/AmdPspRomArmorLib.h>
/*
 *  Return allocated and filled AMD PSP ROM Armor Allow list Table
 *
 *
 * @param[in]  PlatformSpiAllowlist   Pointer to allow list table
 *
 * @return    EFI_SUCCESS
 * @return    EFI_OUT_OF_RESOURCES      Buffer to return couldn't be allocated
 */
EFI_STATUS
EFIAPI
GetPspRomArmorAllowlist (
  IN       SPI_ALLOW_LIST     **PlatformSpiAllowlist
  );
