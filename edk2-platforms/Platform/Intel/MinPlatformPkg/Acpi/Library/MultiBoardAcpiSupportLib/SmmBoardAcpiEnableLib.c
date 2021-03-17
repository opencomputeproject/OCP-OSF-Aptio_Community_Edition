/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BoardAcpiTableLib.h>
#include <Library/MultiBoardAcpiSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>

EFI_STATUS
EFIAPI
BoardEnableAcpi (
  IN BOOLEAN  EnableSci
  )
{
  BOARD_ACPI_ENABLE_FUNC     *BoardAcpiEnableFunc;
  EFI_STATUS                 Status;

  Status = gSmst->SmmLocateProtocol (
                    &gBoardAcpiEnableGuid,
                    NULL,
                    (VOID **)&BoardAcpiEnableFunc
                    );
  if (!EFI_ERROR(Status)) {
    if (BoardAcpiEnableFunc->BoardEnableAcpi != NULL) {
      return BoardAcpiEnableFunc->BoardEnableAcpi (EnableSci);
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardDisableAcpi (
  IN BOOLEAN  DisableSci
  )
{
  BOARD_ACPI_ENABLE_FUNC     *BoardAcpiEnableFunc;
  EFI_STATUS                 Status;

  Status = gSmst->SmmLocateProtocol (
                    &gBoardAcpiEnableGuid,
                    NULL,
                    (VOID **)&BoardAcpiEnableFunc
                    );
  if (!EFI_ERROR(Status)) {
    if (BoardAcpiEnableFunc->BoardDisableAcpi != NULL) {
      return BoardAcpiEnableFunc->BoardDisableAcpi (DisableSci);
    }
  }
  return EFI_SUCCESS;
}

