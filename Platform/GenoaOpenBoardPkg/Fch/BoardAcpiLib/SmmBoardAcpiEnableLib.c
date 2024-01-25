/**
  ACPI enable/disable implementation for BoardAcpiEnableLib library class

  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <SilCommon.h>
#include <FchCore/FchHwAcpi/FchHwAcpiReg.h>
#include <FchHelper.h>
#include <Library/IoLib.h>

#define FCH_ACPI_PM_CONTROL             0x04
#define FCH_ACPI_EVENT_STATUS           0x20
#define FCH_ACPI_EVENT_ENABLE           0x24

EFI_STATUS
EFIAPI
BoardEnableAcpi (
  IN BOOLEAN  EnableSci
  )
{
  UINT16              AcpiPmbase;
  UINT32              Data32;

  AcpiPmbase = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60);

  // Disable all GPE events and clear all GPE status
  Data32 = IoRead32 (AcpiPmbase + FCH_ACPI_EVENT_ENABLE);
  IoWrite32 (AcpiPmbase + FCH_ACPI_EVENT_STATUS, Data32);

  // Set ACPI IRQ to IRQ9 for non-APIC OSes
  IoWrite8 (FCH_IOMAP_REGC00, 0x10);
  IoWrite8 (FCH_IOMAP_REGC01, 9);   // SCI->IRQ9 for PIC
  IoWrite8 (FCH_IOMAP_REGC00, 0x90);
  IoWrite8 (FCH_IOMAP_REGC01, 9);   // SCI->IRQ9 for IOAPIC

  // Finally enable SCI
  IoOr32 (AcpiPmbase + FCH_ACPI_PM_CONTROL, BIT0);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BoardDisableAcpi (
  IN BOOLEAN  DisableSci
  )
{
  UINT16      AcpiPmbase;

  AcpiPmbase = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60);
  MmioAnd32 (AcpiPmbase + FCH_ACPI_PM_CONTROL, (UINT32)~BIT0);  // Switch SCI to SMI

  return EFI_SUCCESS;
}


