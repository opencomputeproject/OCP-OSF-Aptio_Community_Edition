/** @file
  This is an implementation of the ACPI platform driver.

Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

//
// Statements that include other header files
//

#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/WindowsSmmSecurityMitigationTable.h>
#include <Register/Hpet.h>
#include <Guid/EventGroup.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/PciSegmentInfoLib.h>
#include <Library/SortLib.h>
#include <Library/LocalApicLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/PcieResourcesLib.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/MpService.h>
#include <Protocol/PciIo.h>
#include <Register/Cpuid.h>

#define IOAPIC_BASE_ADDR_LO_IOAPIC_MMIO_EN_OFFSET      0
#define IOAPIC_BASE_ADDR_LO_IOAPIC_MMIO_EN_MASK        0x1
// Bitfield Description : Locks the IOAPIC private MMIO address and enable until the next warm reset.
#define IOAPIC_BASE_ADDR_LO_IOAPIC_MMIO_LOCK_OFFSET      1
#define IOAPIC_BASE_ADDR_LO_IOAPIC_MMIO_LOCK_MASK        0x2
// Bitfield Description :
#define IOAPIC_BASE_ADDR_LO_Reserved_7_2_OFFSET      2
#define IOAPIC_BASE_ADDR_LO_Reserved_7_2_MASK        0xfc
// Bitfield Description : IOAPIC private MMIO base address bits 31:8.
#define IOAPIC_BASE_ADDR_LO_IOAPIC_BASE_ADDR_LO_OFFSET      8
#define IOAPIC_BASE_ADDR_LO_IOAPIC_BASE_ADDR_LO_MASK        0xffffff00

#endif
