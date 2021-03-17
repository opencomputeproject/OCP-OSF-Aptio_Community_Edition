/** @file
  Definition of the System Agent global NVS area protocol.
  This protocol publishes the address and format of a global ACPI NVS buffer
  used as a communications buffer between SMM/DXE/PEI code and ASL code.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SYSTEM_AGENT_GLOBAL_NVS_AREA_H_
#define _SYSTEM_AGENT_GLOBAL_NVS_AREA_H_

#include "SaNvs.h"
//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSaGlobalNvsAreaProtocolGuid;

///
/// System Agent Global NVS Area Protocol
///
typedef struct {
  SYSTEM_AGENT_GLOBAL_NVS_AREA *Area;        ///< System Agent Global NVS Area Structure
} SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL;

#endif
