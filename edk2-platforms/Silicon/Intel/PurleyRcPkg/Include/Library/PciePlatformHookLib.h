/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PCIE_PLATFORM_HOOK_LIB_H__
#define __PCIE_PLATFORM_HOOK_LIB_H__

typedef enum {  
  PcieInitStart,
  BeforeBifurcation, 
  AfterBifurcation,             
  BeforePortInit,
  AfterPortInit,
  PcieInitEnd
} PCIE_HOOK_EVENT;

EFI_STATUS
EFIAPI
PciePlatformHookEvent (
  IN PCIE_HOOK_EVENT    Event,
  IN VOID               *Context
  );

#endif
