/** @file
  This is an implementation of the ACPI platform driver.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
#include <Library/BoardAcpiTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/PciSegmentInfoLib.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/MpService.h>
#include <Protocol/PciIo.h>

#include <Register/Cpuid.h>

#endif
