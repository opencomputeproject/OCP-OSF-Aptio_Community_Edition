/** @file
  BMC ACPI SSDT.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
    "BmcSsdt.aml",
    "SSDT",
    0x02,         // SSDT revision.
                  // A Revision field value greater than or equal to 2 signifies that integers 
                  // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL",      // OEM ID (6 byte string)
    "BMCACPI",    // OEM table ID  (8 byte string)
    0x0           // OEM version of DSDT table (4 byte Integer)
    )
{

  External(\_SB.PC00.LPC0, DeviceObj)
  
  Scope (\_SB.PC00.LPC0) 
  {
    #include "IpmiOprRegions.asi"
  } 

}
