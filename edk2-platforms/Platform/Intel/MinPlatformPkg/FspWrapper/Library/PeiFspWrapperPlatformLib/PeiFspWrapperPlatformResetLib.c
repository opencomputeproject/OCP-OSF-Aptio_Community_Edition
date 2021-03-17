/** @file
  Pei library function to reset the system from FSP wrapper.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <FspEas.h>
/**
  Perform platform related reset in FSP wrapper.

  @param[in] ResetType  The type of reset the platform has to perform.

  @return Will reset the system based on Reset status provided.
**/
VOID
EFIAPI
CallFspWrapperResetSystem (
  UINT32    ResetType
  )
{
  EFI_RESET_TYPE             EfiResetType;

  switch (ResetType) {
    case FSP_STATUS_RESET_REQUIRED_COLD:
      EfiResetType = EfiResetCold;
      break;
    case FSP_STATUS_RESET_REQUIRED_WARM:
      EfiResetType = EfiResetWarm;
      break;
    default:
      DEBUG ((DEBUG_ERROR, "UnSupported reset type requested. Initiating cold reset\n"));
      EfiResetType = EfiResetCold;
  }
  (*GetPeiServicesTablePointer ())->ResetSystem2 (EfiResetType, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop ();
}
