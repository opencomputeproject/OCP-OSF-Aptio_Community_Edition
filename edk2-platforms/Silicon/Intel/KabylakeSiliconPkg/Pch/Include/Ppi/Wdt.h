/** @file
  Watchdog Timer PPI

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_WDT_H_
#define _PEI_WDT_H_

#include <Protocol/Wdt.h>
//
// MRC takes a lot of time to execute in debug mode
//
#define WDT_TIMEOUT_BETWEEN_PEI_DXE 60

//
// Extern the GUID for PPI users.
//
extern EFI_GUID       gWdtPpiGuid;

///
/// Reuse WDT_PROTOCOL definition
///
typedef WDT_PROTOCOL  WDT_PPI;

#endif
