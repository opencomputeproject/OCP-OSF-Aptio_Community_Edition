/** @file
    IPMI platform hook.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/IpmiPlatformHookLib.h>

#include <Library/PchCycleDecodingLib.h>
#include <Register/PchRegsLpc.h>
//
// Prototype definitions for IPMI Platform Update Library
//

EFI_STATUS
EFIAPI
PlatformIpmiIoRangeSet(
  UINT16 IpmiIoBase
)
/*++

  Routine Description:

  This function sets IPMI Io range

  Arguments:

   IpmiIoBase

  Returns:

    Status

--*/
{
  return PchLpcGenIoRangeSet((IpmiIoBase & 0xFF0), 0x10, LPC_ESPI_FIRST_SLAVE);
}