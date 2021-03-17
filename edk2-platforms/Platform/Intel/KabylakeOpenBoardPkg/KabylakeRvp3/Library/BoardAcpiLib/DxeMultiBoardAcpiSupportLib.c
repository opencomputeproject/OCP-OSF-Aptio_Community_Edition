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

