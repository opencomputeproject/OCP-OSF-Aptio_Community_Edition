/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/TestPointCheckLib.h>
#include <Library/TestPointLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS
EFIAPI
TestPointCheckPiSignedFvBoot (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "==== TestPointCheckPiSignedFvBoot - Enter\n"));

  DEBUG ((DEBUG_INFO, "==== TestPointCheckPiSignedFvBoot - Exit\n"));
  return EFI_UNSUPPORTED;
}
