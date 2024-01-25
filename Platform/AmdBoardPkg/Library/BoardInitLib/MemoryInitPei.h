/******************************************************************************
 * Copyright (C) 2017-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
Framework PEIM to initialize memory.

Copyright (c) 2013 - 2016 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MEMORY_INIT_PEI_H__
#define __MEMORY_INIT_PEI_H__

#include <PiPei.h>

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Guid/AmdMemoryInfoHob.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/ReadOnlyVariable2.h>

#define MAX_32BIT_ADDRESS                 0xFFFFFFFF
#define MAX_64BIT_ADDRESS                 0xFFFFFFFFFFFFFFFFULL

//
// Function prototypes.
//

EFI_STATUS
EFIAPI
MemoryInit(
  IN      EFI_PEI_SERVICES   **PeiServices
);

EFI_STATUS
InstallEfiMemory(
  IN      EFI_PEI_SERVICES                     **PeiServices,
  IN      EFI_PEI_READ_ONLY_VARIABLE2_PPI      *VariableServices,
  IN      EFI_BOOT_MODE                        BootMode
);

EFI_STATUS
GetRequiredPlatformMemorySize(
  IN      EFI_PEI_SERVICES                     **PeiServices,
  IN      EFI_PEI_READ_ONLY_VARIABLE2_PPI      *VariableServices,
  IN      EFI_BOOT_MODE                        BootMode,
  IN OUT  UINT64                               *MemorySize
);

VOID
ClearMemoryRange(
  IN      UINT64    MemoryRangeBase,
  IN      UINT64    MemoryRangeSize
);

#endif  // __MEMORY_INIT_PEI_H__
