/** @file
  DXE multi-board ACPI table support functionality.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardAcpiTableLib.h>
#include <Library/MultiBoardAcpiSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include <N1xxWUId.h>

EFI_STATUS
EFIAPI
N1xxWUBoardUpdateAcpiTable (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  );

BOARD_ACPI_TABLE_FUNC  mN1xxWUBoardAcpiTableFunc = {
  N1xxWUBoardUpdateAcpiTable
};

EFI_STATUS
EFIAPI
DxeN1xxWUMultiBoardAcpiSupportLibConstructor (
  VOID
  )
{
  if (LibPcdGetSku () == BoardIdN1xxWU) {
    return RegisterBoardAcpiTableFunc (&mN1xxWUBoardAcpiTableFunc);
  }
  return EFI_SUCCESS;
}

