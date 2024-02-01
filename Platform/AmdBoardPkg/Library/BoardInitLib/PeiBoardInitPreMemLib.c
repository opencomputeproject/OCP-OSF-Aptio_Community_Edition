/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file

Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Ppi/PlatformMemorySize.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include "MemoryInitPei.h"

EFI_STATUS
EFIAPI
EndofAmdMemoryInfoHobPpiGuidCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR           mNotifyList = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdMemoryInfoHobPpiGuid,
    EndofAmdMemoryInfoHobPpiGuidCallBack
    };

/**
  This board service detects the board type.

  @retval EFI_SUCCESS   The board was detected successfully.
**/
EFI_STATUS
EFIAPI
BoardDetect (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  This board service initializes board-specific debug devices.

  @retval EFI_SUCCESS   Board-specific debug initialization was successful.
**/
EFI_STATUS
EFIAPI
BoardDebugInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  This board service detects the boot mode.

  @retval EFI_BOOT_MODE The boot mode.
**/
EFI_BOOT_MODE
EFIAPI
BoardBootModeDetect (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}

/**
  A hook for board-specific initialization prior to memory initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
          EFI_STATUS    Various failure values of underlying routines.
**/
EFI_STATUS
EFIAPI
BoardInitBeforeMemoryInit (
  VOID
  )
{
  EFI_STATUS Status;
  Status = PeiServicesNotifyPpi (&mNotifyList);
  ASSERT_EFI_ERROR (Status);
  return (Status);
}

/**
  A hook for board-specific initialization after memory initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
**/
EFI_STATUS
EFIAPI
BoardInitAfterMemoryInit (
  VOID
  )
{
  PcdSet64S (PcdFlashNvStorageVariableBase64,
    (UINT64)(UINTN)PcdGet32 (PcdFlashNvStorageVariableBase));
  PcdSet64S (PcdFlashNvStorageFtwWorkingBase64,
    (UINT64)((UINTN)PcdGet32 (PcdFlashNvStorageVariableBase)
    + FixedPcdGet32 (PcdFlashNvStorageVariableSize)));
  PcdSet64S (PcdFlashNvStorageFtwSpareBase64,
    (UINT64)((UINTN)PcdGet32 (PcdFlashNvStorageVariableBase)
    + FixedPcdGet32 (PcdFlashNvStorageVariableSize)
    + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)));

  DEBUG ((
    DEBUG_INFO,
    " PcdFlashNvStorageVariableBase64 = 0x%lx\n",
    PcdGet64 (PcdFlashNvStorageVariableBase64)
    ));
  DEBUG ((
    DEBUG_INFO,
    " PcdFlashNvStorageFtwWorkingBase64 = 0x%lx\n",
    PcdGet64 (PcdFlashNvStorageFtwWorkingBase64)
    ));
  DEBUG ((
    DEBUG_INFO,
    " PcdFlashNvStorageFtwSpareBase64 = 0x%lx\n",
    PcdGet64 (PcdFlashNvStorageFtwSpareBase64)
    ));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization prior to disabling temporary RAM.

  @retval EFI_SUCCESS   The board initialization was successful.
**/
EFI_STATUS
EFIAPI
BoardInitBeforeTempRamExit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization after disabling temporary RAM.

  @retval EFI_SUCCESS   The board initialization was successful.
**/
EFI_STATUS
EFIAPI
BoardInitAfterTempRamExit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  A Callback routine only AmdMemoryInfoHob is ready.

  @retval EFI_SUCCESS   Platform Pre Memory initialization is successfull.
          EFI_STATUS    Various failure from underlying routine calls.
**/
EFI_STATUS
EFIAPI
EndofAmdMemoryInfoHobPpiGuidCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  PEI_PLATFORM_MEMORY_SIZE_PPI  *PlatformMemorySizePpi;
  EFI_STATUS                    Status;
  UINT64                        MemorySize;

  Status = MemoryInit (PeiServices);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (
            &gPeiPlatformMemorySizePpiGuid,
            0,
            NULL,
            (VOID **) &PlatformMemorySizePpi
            );
  ASSERT_EFI_ERROR (Status);

  Status = PlatformMemorySizePpi->GetPlatformMemorySize (
            PeiServices,
            PlatformMemorySizePpi,
            &MemorySize
            );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Error(%r) in getting Platform Memory size.\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }
  DEBUG ((
    DEBUG_INFO,
    "Installing PeiMemory, BaseAddress = 0x%x, Size = 0x%x\n",
    0,
    MemorySize
    ));
  Status = PeiServicesInstallPeiMemory (0, MemorySize);
  ASSERT_EFI_ERROR(Status);
  return Status;
}
