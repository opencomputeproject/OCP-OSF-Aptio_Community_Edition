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
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

VOID
DumpCharArray (
  IN CHAR8  *Ch,
  IN UINTN  Size
  );

VOID
DumpAcpiTableHeader (
  IN EFI_ACPI_DESCRIPTION_HEADER  *Table
  );

BOOLEAN
IsMmioExit (
  IN EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN UINT64                Length,
  IN BOOLEAN               CheckAllocated
  );

VOID
DumpAcpiHpet (
  IN EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER  *Hpet
  )
{
  DumpAcpiTableHeader (&Hpet->Header);

  DEBUG ((DEBUG_INFO, "         "));
  DEBUG ((DEBUG_INFO, " Timer :"));
  DEBUG ((DEBUG_INFO, " BlockId=0x%08x", Hpet->EventTimerBlockId));
  DEBUG ((DEBUG_INFO, " BaseAddress=0x%016lx", Hpet->BaseAddressLower32Bit.Address));
  DEBUG ((DEBUG_INFO, " HpetNumber=0x%08x", Hpet->HpetNumber));
  DEBUG ((DEBUG_INFO, " MinClockTick=0x%04x", Hpet->MainCounterMinimumClockTickInPeriodicMode));
  DEBUG ((DEBUG_INFO, " PageProtection=0x%02x", Hpet->PageProtectionAndOemAttribute));
  DEBUG ((DEBUG_INFO, "\n"));
}

EFI_STATUS
CheckAcpiHpet (
  IN EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER  *Hpet
  )
{
  if (!IsMmioExit (Hpet->BaseAddressLower32Bit.Address, SIZE_4KB, TRUE)) {
    DEBUG ((DEBUG_ERROR, "HPET resource (0x%x) is not reported correctly.\n", Hpet->BaseAddressLower32Bit.Address));
    return EFI_NOT_STARTED;
  }
  return EFI_SUCCESS;
}
