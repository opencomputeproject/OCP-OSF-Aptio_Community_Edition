/** @file
  Board pre-memory initialization.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
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
#include <Library/MultiBoardInitSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include <N1xxWUId.h>

EFI_STATUS
EFIAPI
N1xxWUBoardDetect (
  VOID
  );

EFI_STATUS
EFIAPI
N1xxWUMultiBoardDetect (
  VOID
  );

EFI_BOOT_MODE
EFIAPI
N1xxWUBoardBootModeDetect (
  VOID
  );

EFI_STATUS
EFIAPI
N1xxWUBoardDebugInit (
  VOID
  );

EFI_STATUS
EFIAPI
N1xxWUBoardInitBeforeMemoryInit (
  VOID
  );

BOARD_DETECT_FUNC  mN1xxWUBoardDetectFunc = {
  N1xxWUMultiBoardDetect
};

BOARD_PRE_MEM_INIT_FUNC  mN1xxWUBoardPreMemInitFunc = {
  N1xxWUBoardDebugInit,
  N1xxWUBoardBootModeDetect,
  N1xxWUBoardInitBeforeMemoryInit,
  NULL, // BoardInitAfterMemoryInit
  NULL, // BoardInitBeforeTempRamExit
  NULL, // BoardInitAfterTempRamExit
};

EFI_STATUS
EFIAPI
N1xxWUMultiBoardDetect (
  VOID
  )
{
  N1xxWUBoardDetect ();
  if (LibPcdGetSku () == BoardIdN1xxWU) {
    RegisterBoardPreMemInit (&mN1xxWUBoardPreMemInitFunc);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PeiN1xxWUMultiBoardInitPreMemLibConstructor (
  VOID
  )
{
  return RegisterBoardDetect (&mN1xxWUBoardDetectFunc);
}