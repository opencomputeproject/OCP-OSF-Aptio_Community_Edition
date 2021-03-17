/** @file
  Provide SecTemporaryRamDone function.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>

#include <Ppi/TemporaryRamDone.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/FspWrapperPlatformLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/BoardInitLib.h>

/**
This interface disables temporary memory in SEC Phase.
**/
VOID
EFIAPI
SecPlatformDisableTemporaryMemory (
  VOID
  )
{
  EFI_STATUS                Status;
  VOID                      *TempRamExitParam;

  DEBUG((DEBUG_INFO, "SecPlatformDisableTemporaryMemory enter\n"));
  
  Status = BoardInitBeforeTempRamExit ();
  ASSERT_EFI_ERROR (Status);

  TempRamExitParam = UpdateTempRamExitParam ();
  Status = CallTempRamExit (TempRamExitParam);
  DEBUG((DEBUG_INFO, "TempRamExit status: 0x%x\n", Status));
  ASSERT_EFI_ERROR(Status);
  
  Status = BoardInitAfterTempRamExit ();
  ASSERT_EFI_ERROR (Status);

  return ;
}
