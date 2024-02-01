/*****************************************************************************
 * Copyright (C) 2022 - 2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file

SetCacheMtrr library functions.
This implementation is for typical platforms and may not be
needed when cache MTRR will be initialized by FSP.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/MtrrLib.h>

/**
  Set Cache Mtrr.
**/
VOID
EFIAPI
SetCacheMtrr (
  VOID
  )
{
  EFI_STATUS  Status;

  // VGA MMIO
  Status = MtrrSetMemoryAttribute (
             0xA0000,
             0x20000,
             CacheUncacheable
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Error(%r) in setting CacheUncacheable for 0xA0000-0xBFFFF\n",
      Status
      ));
  }

  // TPM OS boot hang fix
  Status = MtrrSetMemoryAttribute (
             0xC0000,
             0x40000,
             CacheWriteProtected
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Error(%r) in setting CacheWriteProtected for 0xC0000-0x40000\n",
      Status
      ));
  }

  return;
}

/**
  Update MTRR setting in EndOfPei phase.
  This function will clear temporary memory (CAR) phase MTRR settings
  and configure MTRR to cover permanent memory.

  @retval  EFI_SUCCESS  The function completes successfully.
  @retval  Others       Some error occurs.
**/
EFI_STATUS
EFIAPI
SetCacheMtrrAfterEndOfPei (
  VOID
  )
{
  return EFI_SUCCESS;
}
