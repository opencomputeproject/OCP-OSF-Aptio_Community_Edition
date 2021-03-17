/** @file
  Provide FSP wrapper platform related function.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/SiliconPolicyInitLib.h>
#include <Library/SiliconPolicyUpdateLib.h>


/**
  This function overrides the default configurations in the FSP-M UPD data region.

  @note At this point, memory is NOT ready, PeiServices are available to use.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspmUpdData (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  SiliconPolicyInitPreMem (FspUpdRgnPtr);
  SiliconPolicyUpdatePreMem (FspUpdRgnPtr);
  SiliconPolicyDonePreMem (FspUpdRgnPtr);
}

/**
  This function overrides the default configurations in the FSP-S UPD data region.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspsUpdData (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  SiliconPolicyInitPostMem (FspUpdRgnPtr);
  SiliconPolicyUpdatePostMem (FspUpdRgnPtr);
  SiliconPolicyDonePostMem (FspUpdRgnPtr);
}

/**
  Update TempRamExit parameter.

  @note At this point, memory is ready, PeiServices are available to use.

  @return TempRamExit parameter.
**/
VOID *
EFIAPI
UpdateTempRamExitParam (
  VOID
  )
{
  return NULL;
}
