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
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
RegisterBoardAcpiTableFunc (
  IN BOARD_ACPI_TABLE_FUNC  *BoardAcpiTableFunc
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gBoardAcpiTableGuid,
                  EFI_NATIVE_INTERFACE,
                  BoardAcpiTableFunc
                  );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
