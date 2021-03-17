/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
//*                                                                     *
//*      All rights reserved.                                           *
//*                                                                     *
//*      This program and the accompanying materials are licensed and   *
//*      made available under the terms and conditions of the BSD       *
//*      License that accompanies this distribution.  The full text of  *
//*      the license may be found at:                                   *
//*      http://opensource.org/licenses/bsd-license.php.                *
//*                                                                     *
//*      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
//*      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
//*      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
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
