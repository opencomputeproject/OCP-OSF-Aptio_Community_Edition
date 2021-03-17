/** @file
  ACPI WSMT table

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// Statements that include other files
//

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/WindowsSmmSecurityMitigationTable.h>
#include <Library/PcdLib.h>

//
// WSMT Definitions
//

#define EFI_ACPI_OEM_WSMT_REVISION                      0x00000001

EFI_ACPI_WSMT_TABLE Wsmt = {
  {
    EFI_ACPI_WINDOWS_SMM_SECURITY_MITIGATION_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_WSMT_TABLE),
    EFI_WSMT_TABLE_REVISION,

    //
    // Checksum will be updated at runtime
    //
    0x00,

    //
    // It is expected that these values will be updated at runtime
    //
    { ' ', ' ', ' ', ' ', ' ', ' ' },

    0,
    EFI_ACPI_OEM_WSMT_REVISION,
    0,
    0
  },

  FixedPcdGet32(PcdWsmtProtectionFlags)
};
