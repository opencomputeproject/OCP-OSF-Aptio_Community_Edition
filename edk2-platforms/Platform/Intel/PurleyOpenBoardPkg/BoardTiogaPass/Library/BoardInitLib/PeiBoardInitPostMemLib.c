/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
//*                                                                     *
//*      All rights reserved.                                           *
//*                                                                     *
//*      SPDX-License-Identifier: BSD-2-Clause-Patent                   *
//*                                                                     *
//***********************************************************************

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
TiogaPassBoardInitBeforeSiliconInit (
  VOID
  );

EFI_STATUS
EFIAPI
TiogaPassBoardInitAfterSiliconInit (
  VOID
  );

EFI_STATUS
EFIAPI
BoardInitBeforeSiliconInit (
  VOID
  )
{
  TiogaPassBoardInitBeforeSiliconInit ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardInitAfterSiliconInit (
  VOID
  )
{
  TiogaPassBoardInitAfterSiliconInit ();
  return EFI_SUCCESS;
}
