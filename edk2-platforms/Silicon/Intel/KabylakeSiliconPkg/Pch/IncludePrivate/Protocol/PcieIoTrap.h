/** @file
  This file defines the PCH PCIE IoTrap Protocol.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_PCIE_IOTRAP_H_
#define _PCH_PCIE_IOTRAP_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gPchPcieIoTrapProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_PCIE_IOTRAP_PROTOCOL PCH_PCIE_IOTRAP_PROTOCOL;

///
/// Pcie Trap valid types
///
typedef enum {
  PciePmTrap,
  PcieTrapTypeMaximum
} PCH_PCIE_TRAP_TYPE;

/**
 This protocol is used to provide the IoTrap address to trigger PCH PCIE call back events
**/
struct _PCH_PCIE_IOTRAP_PROTOCOL {
  UINT16      PcieTrapAddress;
};

#endif
