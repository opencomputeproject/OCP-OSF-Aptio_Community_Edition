/** @file
  This file contains a structure definition for the ACPI 5.0 Firmware ACPI
  Control Structure (FACS).  The contents of this file should only be modified
  for bug fixes, no porting is required.

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

//
// FACS Definitions
//
#define EFI_ACPI_FIRMWARE_WAKING_VECTOR 0x00000000
#define EFI_ACPI_GLOBAL_LOCK            0x00000000

//
// Firmware Control Structure Feature Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_FIRMWARE_CONTROL_STRUCTURE_FLAGS 0x00000000

#define EFI_ACPI_X_FIRMWARE_WAKING_VECTOR         0x0000000000000000

#define EFI_ACPI_OSPM_FLAGS                       0x00000000


//
// Firmware ACPI Control Structure
// Please modify all values in Facs.h only.
//

EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE Facs = {
  EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE,
  sizeof (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE),

  //
  // Hardware Signature will be updated at runtime
  //
  0x00000000,

  EFI_ACPI_FIRMWARE_WAKING_VECTOR,
  EFI_ACPI_GLOBAL_LOCK,
  EFI_ACPI_FIRMWARE_CONTROL_STRUCTURE_FLAGS,
  EFI_ACPI_X_FIRMWARE_WAKING_VECTOR,
  EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_VERSION,
  {
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE
  },
  EFI_ACPI_OSPM_FLAGS,
  {
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE,
    EFI_ACPI_RESERVED_BYTE
  }
};
