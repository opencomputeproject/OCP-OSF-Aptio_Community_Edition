/** @file
  Platform Hook Library instances

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardAcpiTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include <N1xxWUId.h>

EFI_STATUS
EFIAPI
N1xxWUBoardEnableAcpi (
  IN BOOLEAN  EnableSci
  )
{
  // enable additional board register
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
N1xxWUBoardDisableAcpi (
  IN BOOLEAN  DisableSci
  )
{
  // enable additional board register
  return EFI_SUCCESS;
}

