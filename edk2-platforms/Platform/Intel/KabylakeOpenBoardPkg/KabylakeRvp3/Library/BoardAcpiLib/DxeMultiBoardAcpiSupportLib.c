/** @file
  Platform Hook Library instances

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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

#include <KabylakeRvp3Id.h>

EFI_STATUS
EFIAPI
KabylakeRvp3BoardUpdateAcpiTable (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  );

BOARD_ACPI_TABLE_FUNC  mKabylakeRvp3BoardAcpiTableFunc = {
  KabylakeRvp3BoardUpdateAcpiTable
};

EFI_STATUS
EFIAPI
DxeKabylakeRvp3MultiBoardAcpiSupportLibConstructor (
  VOID
  )
{
  if (LibPcdGetSku () == BoardIdKabyLakeYLpddr3Rvp3) {
    return RegisterBoardAcpiTableFunc (&mKabylakeRvp3BoardAcpiTableFunc);
  }
  return EFI_SUCCESS;
}

