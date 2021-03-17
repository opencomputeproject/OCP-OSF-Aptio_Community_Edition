/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef PCI_PLATFORM_HOOKS_H_
#define PCI_PLATFORM_HOOKS_H_

VOID
ChipsetCallback (
  IN  EFI_HANDLE                                    RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS   PciAddress,
  IN  EFI_PCI_ENUMERATION_PHASE                     Phase,
  EFI_PCI_CALLBACK_CONTEXT                          *Context
  );

EFI_STATUS
PciTreeTraverse (
  IN  UINT8                                        StartBus
  );

#endif
