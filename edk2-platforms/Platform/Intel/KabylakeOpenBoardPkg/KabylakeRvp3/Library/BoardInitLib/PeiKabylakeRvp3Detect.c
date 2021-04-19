/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <SaPolicyCommon.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/PeiSaPolicyLib.h>
#include <Library/BoardInitLib.h>
#include <PchAccess.h>
#include <Library/GpioNativeLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/GpioExpanderLib.h>
#include <SioRegs.h>
#include <Library/PchPcrLib.h>
#include <Library/SiliconInitLib.h>

#include "PeiKabylakeRvp3InitLib.h"

#include <ConfigBlock.h>
#include <ConfigBlock/MemoryConfig.h>

BOOLEAN
IsKabylakeRvp3 (
  VOID
  )
{
  // TBD: Do detection - BoardIdKabylakeRvp3 v.s. BoardIdKabyLakeYLpddr3Rvp3
  return TRUE;
}

EFI_STATUS
EFIAPI
KabylakeRvp3BoardDetect (
  VOID
  )
{
  if (LibPcdGetSku () != 0) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "KabylakeRvp3DetectionCallback\n"));

  if (IsKabylakeRvp3 ()) {
    LibPcdSetSku (BoardIdKabyLakeYLpddr3Rvp3);

    DEBUG ((DEBUG_INFO, "SKU_ID: 0x%x\n", LibPcdGetSku()));
    ASSERT (LibPcdGetSku() == BoardIdKabyLakeYLpddr3Rvp3);
  }
  return EFI_SUCCESS;
}
