/** @file
  SMM multi-board ACPI support functionality.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardAcpiEnableLib.h>
#include <Library/MultiBoardAcpiSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include <N1xxWUId.h>

EFI_STATUS
EFIAPI
N1xxWUBoardEnableAcpi (
  IN BOOLEAN  EnableSci
  );

EFI_STATUS
EFIAPI
N1xxWUBoardDisableAcpi (
  IN BOOLEAN  DisableSci
  );

EFI_STATUS
EFIAPI
SiliconEnableAcpi (
  IN BOOLEAN  EnableSci
  );

EFI_STATUS
EFIAPI
SiliconDisableAcpi (
  IN BOOLEAN  DisableSci
  );

EFI_STATUS
EFIAPI
N1xxWUMultiBoardEnableAcpi (
  IN BOOLEAN  EnableSci
  )
{
  SiliconEnableAcpi (EnableSci);
  return N1xxWUBoardEnableAcpi (EnableSci);
}

EFI_STATUS
EFIAPI
N1xxWUMultiBoardDisableAcpi (
  IN BOOLEAN  DisableSci
  )
{
  SiliconDisableAcpi (DisableSci);
  return N1xxWUBoardDisableAcpi (DisableSci);
}

BOARD_ACPI_ENABLE_FUNC  mN1xxWUBoardAcpiEnableFunc = {
  N1xxWUMultiBoardEnableAcpi,
  N1xxWUMultiBoardDisableAcpi,
};

EFI_STATUS
EFIAPI
SmmN1xxWUMultiBoardAcpiSupportLibConstructor (
  VOID
  )
{
  if (LibPcdGetSku () == BoardIdN1xxWU) {
    return RegisterBoardAcpiEnableFunc (&mN1xxWUBoardAcpiEnableFunc);
  }
  return EFI_SUCCESS;
}

