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
#include <Library/MultiBoardInitSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include <KabylakeRvp3Id.h>

EFI_STATUS
EFIAPI
KabylakeRvp3BoardDetect (
  VOID
  );

EFI_STATUS
EFIAPI
KabylakeRvp3MultiBoardDetect (
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

BOARD_DETECT_FUNC  mKabylakeRvp3BoardDetectFunc = {
  KabylakeRvp3MultiBoardDetect
};

BOARD_PRE_MEM_INIT_FUNC  mKabylakeRvp3BoardPreMemInitFunc = {
  KabylakeRvp3BoardDebugInit,
  KabylakeRvp3BoardBootModeDetect,
  KabylakeRvp3BoardInitBeforeMemoryInit,
  NULL, // BoardInitAfterMemoryInit
  NULL, // BoardInitBeforeTempRamExit
  NULL, // BoardInitAfterTempRamExit
};

EFI_STATUS
EFIAPI
KabylakeRvp3MultiBoardDetect (
  VOID
  )
{
  KabylakeRvp3BoardDetect ();
  if (LibPcdGetSku () == BoardIdKabyLakeYLpddr3Rvp3) {
    RegisterBoardPreMemInit (&mKabylakeRvp3BoardPreMemInitFunc);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PeiKabylakeRvp3MultiBoardInitPreMemLibConstructor (
  VOID
  )
{
  return RegisterBoardDetect (&mKabylakeRvp3BoardDetectFunc);
}