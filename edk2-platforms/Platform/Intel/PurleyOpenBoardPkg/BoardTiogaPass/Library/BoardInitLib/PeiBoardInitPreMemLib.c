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
TiogaPassBoardDetect (
  VOID
  );

EFI_BOOT_MODE
EFIAPI
TiogaPassBoardBootModeDetect (
  VOID
  );

EFI_STATUS
EFIAPI
TiogaPassBoardDebugInit (
  VOID
  );

EFI_STATUS
EFIAPI
TiogaPassBoardInitBeforeMemoryInit (
  VOID
  );

EFI_STATUS
EFIAPI
TiogaPassBoardInitAfterMemoryInit (
  VOID
  );

EFI_STATUS
EFIAPI
BoardDetect (
  VOID
  )
{
  TiogaPassBoardDetect ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardDebugInit (
  VOID
  )
{
  TiogaPassBoardDebugInit ();
  return EFI_SUCCESS;
}

EFI_BOOT_MODE
EFIAPI
BoardBootModeDetect (
  VOID
  )
{
  return TiogaPassBoardBootModeDetect ();
}

EFI_STATUS
EFIAPI
BoardInitBeforeMemoryInit (
  VOID
  )
{
  TiogaPassBoardInitBeforeMemoryInit ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardInitAfterMemoryInit (
  VOID
  )
{
  TiogaPassBoardInitAfterMemoryInit ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardInitBeforeTempRamExit (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardInitAfterTempRamExit (
  VOID
  )
{
  return EFI_SUCCESS;
}

