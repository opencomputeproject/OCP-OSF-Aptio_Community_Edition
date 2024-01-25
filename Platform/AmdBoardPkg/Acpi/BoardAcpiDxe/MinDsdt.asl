/*****************************************************************************
 *
 * Copyright (C) 2019-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/* This file includes code originally published under the following license. */

/** @file
  ACPI minimum DSDT table

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02, // DSDT revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
  "AMD   ",   // OEM ID (6 byte string)
  "AMD_EDK2",// OEM table ID  (8 byte string)
  0x00 // OEM version of DSDT table (4 byte Integer)
)

// BEGIN OF ASL SCOPE
{
  Scope (\_SB) {

    Device (AMDM) {
      Name (_HID, EISAID ("PNP0C02"))
      Name (_UID, Zero)
      Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
      {
         QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
         0x0000000000000000,
         FixedPcdGet64 (PcdPciExpressBaseAddress),
         FixedPcdGet64 (PcdPciExpressLimitAddress),
         0x0000000000000000,
         FixedPcdGet64 (PcdPciExpressBaseSize),
         ,, , AddressRangeMemory, TypeStatic)
      })
    }
  }
  Name (\_S5, Package(4) {
    0x05, 0x00, 0x00, 0x00 // PM1a_CNT.SLP_TYP = 5, PM1b_CNT.SLP_TYP = 0
  })
  Include ("Fch9004I2C_I3C.asl")
}// End of ASL File

