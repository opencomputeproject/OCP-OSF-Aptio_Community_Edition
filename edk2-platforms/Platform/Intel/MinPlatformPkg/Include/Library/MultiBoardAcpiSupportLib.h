/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MULTI_BOARD_ACPI_SUPPORT_LIB_H_
#define _MULTI_BOARD_ACPI_SUPPORT_LIB_H_

#include <Library/BoardAcpiTableLib.h>
#include <Library/BoardAcpiEnableLib.h>

typedef
EFI_STATUS
(EFIAPI *BOARD_ENABLE_ACPI) (
  IN BOOLEAN  EnableSci
  );

typedef
EFI_STATUS
(EFIAPI *BOARD_DISABLE_ACPI) (
  IN BOOLEAN  DisableSci
  );

typedef struct {
  BOARD_ENABLE_ACPI        BoardEnableAcpi;
  BOARD_DISABLE_ACPI       BoardDisableAcpi;
} BOARD_ACPI_ENABLE_FUNC;

typedef
EFI_STATUS
(EFIAPI *BOARD_UPDATE_ACPI_TABLE) (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  );

typedef struct {
  BOARD_UPDATE_ACPI_TABLE  BoardUpdateAcpiTable;
} BOARD_ACPI_TABLE_FUNC;

EFI_STATUS
EFIAPI
RegisterBoardAcpiEnableFunc (
  IN BOARD_ACPI_ENABLE_FUNC  *BoardAcpiEnableFunc
  );

EFI_STATUS
EFIAPI
RegisterBoardAcpiTableFunc (
  IN BOARD_ACPI_TABLE_FUNC  *BoardAcpiTableFunc
  );

#endif
