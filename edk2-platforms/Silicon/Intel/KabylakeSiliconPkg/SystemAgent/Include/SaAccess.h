/** @file
  Macros to simplify and abstract the interface to PCI configuration.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SAACCESS_H_
#define _SAACCESS_H_

#include "SaRegs.h"

///
/// SystemAgent Base Address definition
///
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND  1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND  1000
#endif

//
// SA PCI Express* Port configuration
//
#define SA_PEG_BUS_NUM     0x00
#define SA_PEG_DEV_NUM     0x01
#define SA_PEG10_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG10_FUN_NUM   0x00
#define SA_PEG11_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG11_FUN_NUM   0x01
#define SA_PEG12_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG12_FUN_NUM   0x02
#define SA_PEG_MAX_FUN     0x03

///
/// SgMode settings
///
typedef enum {
  SgModeDisabled = 0,
  SgModeReserved,
  SgModeMuxless,
  SgModeDgpu,
  SgModeMax
} SG_MODE;
#endif
