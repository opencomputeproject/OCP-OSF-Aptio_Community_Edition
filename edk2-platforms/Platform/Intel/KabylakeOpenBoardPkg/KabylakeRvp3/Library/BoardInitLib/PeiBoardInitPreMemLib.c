/** @file
  Platform Hook Library instances

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
KabylakeRvp3BoardDetect (
  VOID
  );

EFI_BOOT_MODE
EFIAPI
KabylakeRvp3BoardBootModeDetect (
  VOID
  );

EFI_STATUS
EFIAPI
KabylakeRvp3BoardDebugInit (
  VOID
  );

EFI_STATUS
EFIAPI
KabylakeRvp3BoardInitBeforeMemoryInit (
  VOID
  );

EFI_STATUS
EFIAPI
BoardDetect (
  VOID
  )
{
  KabylakeRvp3BoardDetect ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardDebugInit (
  VOID
  )
{
  KabylakeRvp3BoardDebugInit ();
  return EFI_SUCCESS;
}

EFI_BOOT_MODE
EFIAPI
BoardBootModeDetect (
  VOID
  )
{
  return KabylakeRvp3BoardBootModeDetect ();
}

EFI_STATUS
EFIAPI
BoardInitBeforeMemoryInit (
  VOID
  )
{
  KabylakeRvp3BoardInitBeforeMemoryInit ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardInitAfterMemoryInit (
  VOID
  )
{
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

